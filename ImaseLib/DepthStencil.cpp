//--------------------------------------------------------------------------------------
// File: DepthStencil.cpp
//
// Helper for managing offscreen render targets
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//-------------------------------------------------------------------------------------

#include "pch.h"
#include "DepthStencil.h"

using namespace DirectX;
using namespace Imase;

DepthStencil::DepthStencil(DXGI_FORMAT format) noexcept :
    m_format(format),
    m_width(0),
    m_height(0)
{
}

void DepthStencil::SetDevice(_In_ ID3D11Device* device)
{
    if (device == m_device.Get())
        return;

    if (m_device)
    {
        ReleaseDevice();
    }

    {
        UINT formatSupport = 0;
        if (FAILED(device->CheckFormatSupport(m_format, &formatSupport)))
        {
            throw std::runtime_error("CheckFormatSupport");
        }

        constexpr UINT32 required = D3D11_FORMAT_SUPPORT_TEXTURE2D | D3D11_FORMAT_SUPPORT_DEPTH_STENCIL;
        if ((formatSupport & required) != required)
        {
#ifdef _DEBUG
            char buff[128] = {};
            sprintf_s(buff, "DepthStencil: Device does not support the requested format (%u)!\n", m_format);
            OutputDebugStringA(buff);
#endif
            throw std::runtime_error("DepthStencil");
        }
    }

    m_device = device;
}


void DepthStencil::SizeResources(size_t width, size_t height)
{
    if (width == m_width && height == m_height)
        return;

    if (m_width > UINT32_MAX || m_height > UINT32_MAX)
    {
        throw std::out_of_range("Invalid width/height");
    }

    if (!m_device)
        return;

    m_width = m_height = 0;

     // Create a depth stencil view for use with 3D rendering if needed.
    CD3D11_TEXTURE2D_DESC depthStencilDesc(
        m_format,
        static_cast<UINT>(width),
        static_cast<UINT>(height),
        1, // This depth stencil view has only one texture.
        1, // Use a single mipmap level.
        D3D11_BIND_DEPTH_STENCIL
    );

    DX::ThrowIfFailed(m_device->CreateTexture2D(
        &depthStencilDesc,
        nullptr,
        m_texture2D.ReleaseAndGetAddressOf()
    ));

    DX::ThrowIfFailed(m_device->CreateDepthStencilView(
        m_texture2D.Get(),
        nullptr,
        m_depthStencilView.ReleaseAndGetAddressOf()
    ));

    m_width = width;
    m_height = height;
}


void DepthStencil::ReleaseDevice() noexcept
{
    m_depthStencilView.Reset();
    m_texture2D.Reset();

    m_device.Reset();

    m_width = m_height = 0;
}

void DepthStencil::SetWindow(const RECT& output)
{
    // Determine the render target size in pixels.
    auto const width = size_t(std::max<LONG>(output.right - output.left, 1));
    auto const height = size_t(std::max<LONG>(output.bottom - output.top, 1));

    SizeResources(width, height);
}
