//--------------------------------------------------------------------------------------
// File: ShadowMap.cpp
//
// シャドウマップクラス
//
// Date: 2022.6.19
// Author: Hideyasu Imase
//--------------------------------------------------------------------------------------
#include "pch.h"
#include "ShadowMap.h"
#include "VertexTypes.h"
#include "ReadData.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace Imase;

// 深度バイアス（初期値 0.1 バイアス値の計算は m_offset / ライトの影響範囲）
const float ShadowMap::DEFALT_OFFSET = 0.2f;

// シャドウマップ作成時のニアークリップの値
const float ShadowMap::NEAR_CLIP = 0.1f;

ShadowMap::ShadowMap()
	: m_gaussianFilterEnable(true)
	, m_vsmFilterEnable(true)
	, m_resolution(ShadowMap::DEFALT_RESOLUTION)
	, m_viewPort{}
	, m_offset(ShadowMap::DEFALT_OFFSET)
	, m_path{}
{
}

// 初期化関数
void ShadowMap::Initialize(ID3D11Device* device, const wchar_t* path, UINT resolution)
{
	SetDirectory(path);

	// シャドウマップの解像度
	m_resolution = resolution;

	// シェーダーの読み込み
	std::vector<uint8_t> vs_ShadowMap = DX::ReadData(GetFullName(L"SM_VS_Depth.cso"));
	std::vector<uint8_t> ps_ShadowMap = DX::ReadData(GetFullName(L"SM_PS_Depth.cso"));
	std::vector<uint8_t> vs_ShadowModel = DX::ReadData(GetFullName(L"SM_VS.cso"));
	std::vector<uint8_t> ps_ShadowModel = DX::ReadData(GetFullName(L"SM_PS.cso"));
	std::vector<uint8_t> ps_ShadowModel_Tex = DX::ReadData(GetFullName(L"SM_PS_Texture.cso"));

	// シェーダーの作成
	device->CreateVertexShader(vs_ShadowMap.data(), vs_ShadowMap.size(), nullptr, m_VS_ShadowMap.ReleaseAndGetAddressOf());
	device->CreatePixelShader(ps_ShadowMap.data(), ps_ShadowMap.size(), nullptr, m_PS_ShadowMap.ReleaseAndGetAddressOf());
	device->CreateVertexShader(vs_ShadowModel.data(), vs_ShadowModel.size(), nullptr, m_VS_ShadowModel.ReleaseAndGetAddressOf());
	device->CreatePixelShader(ps_ShadowModel.data(), ps_ShadowModel.size(), nullptr, m_PS_ShadowModel.ReleaseAndGetAddressOf());
	device->CreatePixelShader(ps_ShadowModel_Tex.data(), ps_ShadowModel_Tex.size(), nullptr, m_PS_ShadowModel_Texture.ReleaseAndGetAddressOf());

	// 入力レイアウトの作成
	DX::ThrowIfFailed(
		device->CreateInputLayout(
			VertexPositionNormalTangentColorTexture::InputElements, VertexPositionNormalTangentColorTexture::InputElementCount,
			vs_ShadowModel.data(), vs_ShadowModel.size(),
			m_shadowMapInputLayout.GetAddressOf()
		)
	);

	{
		// テクスチャの作成（深度バッファ用）
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = m_resolution;   // 幅
		desc.Height = m_resolution;  // 高さ
		desc.MipLevels = 1;       // ミップマップ レベル数
		desc.ArraySize = 1;       // 配列サイズ
		desc.Format = DXGI_FORMAT_D16_UNORM;  // フォーマット
		desc.SampleDesc.Count = 1;  // マルチサンプリングの設定
		desc.SampleDesc.Quality = 0;  // マルチサンプリングの品質
		desc.Usage = D3D11_USAGE_DEFAULT;      // デフォルト使用法
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL; // 深度バッファとして使用
		desc.CPUAccessFlags = 0;   // CPUからはアクセスしない
		desc.MiscFlags = 0;   // その他の設定なし
		DX::ThrowIfFailed(device->CreateTexture2D(&desc, nullptr, m_depathTexture2D.GetAddressOf()));
	}

	{
		// テクスチャの作成（シャドウマップ作成用）
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = m_resolution;   // 幅
		desc.Height = m_resolution;  // 高さ
		desc.MipLevels = 1;       // ミップマップ レベル数
		desc.ArraySize = 1;       // 配列サイズ
		desc.Format = DXGI_FORMAT_R16G16_FLOAT;  // フォーマット
		desc.SampleDesc.Count = 1;  // マルチサンプリングの設定
		desc.SampleDesc.Quality = 0;  // マルチサンプリングの品質
		desc.Usage = D3D11_USAGE_DEFAULT;      // デフォルト使用法
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; // 深度/ステンシル、シェーダ リソース ビューとして使用
		desc.CPUAccessFlags = 0;   // CPUからはアクセスしない
		desc.MiscFlags = 0;   // その他の設定なし
		// 深度書き込み用
		DX::ThrowIfFailed(device->CreateTexture2D(&desc, nullptr, m_shadowMapTexture2D.GetAddressOf()));

	}

	{
		// 深度/ステンシル ビューの作成
		D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
		desc.Format = DXGI_FORMAT_D16_UNORM;
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		desc.Flags = 0;
		desc.Texture2D.MipSlice = 0;
		DX::ThrowIfFailed(device->CreateDepthStencilView(m_depathTexture2D.Get(), &desc, m_DSV.GetAddressOf()));
	}

	{
		// レンダーターゲットビューの設定
		D3D11_RENDER_TARGET_VIEW_DESC desc;
		desc.Format = DXGI_FORMAT_R16G16_FLOAT;
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;
		DX::ThrowIfFailed(device->CreateRenderTargetView(m_shadowMapTexture2D.Get(), &desc, m_shadowMapRTV.GetAddressOf()));
	}

	{
		// シェーダリソースビューの作成
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		desc.Format = DXGI_FORMAT_R16G16_FLOAT; // フォーマット
		desc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;  // 2Dテクスチャ
		desc.Texture2D.MostDetailedMip = 0;   // 最初のミップマップ レベル
		desc.Texture2D.MipLevels = static_cast<UINT>(-1);  // すべてのミップマップ レベル
		DX::ThrowIfFailed(device->CreateShaderResourceView(m_shadowMapTexture2D.Get(), &desc, m_shadowMapSRV.GetAddressOf()));
	}

	// ビューポート
	m_viewPort.TopLeftX = 0.0f;		// ビューポート領域の左上X座標。
	m_viewPort.TopLeftY = 0.0f;		// ビューポート領域の左上Y座標。
	m_viewPort.Width = static_cast<FLOAT>(m_resolution);	// ビューポート領域の幅
	m_viewPort.Height = static_cast<FLOAT>(m_resolution);	// ビューポート領域の高さ
	m_viewPort.MinDepth = 0.0f;		// ビューポート領域の深度値の最小値
	m_viewPort.MaxDepth = 1.0f;		// ビューポート領域の深度値の最大値

	// 定数バッファの作成
	{
		D3D11_BUFFER_DESC desc = {};
		// バッファサイズは１６の倍数でないといけない
		size_t size = sizeof(ConstantBuffer);
		if (size % 16) size++;
		desc.ByteWidth = static_cast<UINT>(size * 16);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		DX::ThrowIfFailed(device->CreateBuffer(&desc, nullptr, m_shadowMapConstantBuffer.GetAddressOf()));
	}

	{
		// シャドウマップ用サンプラーの作成
		D3D11_SAMPLER_DESC desc;
		desc.Filter = D3D11_FILTER_ANISOTROPIC;
		desc.MipLODBias = 0.0f;
		desc.MaxAnisotropy = 2;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = FLT_MAX;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.BorderColor[0] = 1.0f;
		desc.BorderColor[1] = 1.0f;
		desc.BorderColor[2] = 1.0f;
		desc.BorderColor[3] = 1.0f;
		DX::ThrowIfFailed(device->CreateSamplerState(&desc, m_sampler.GetAddressOf()));
	}

}

void ShadowMap::Begin(
	ID3D11DeviceContext* context,
	DirectX::SimpleMath::Vector3 lightPos,
	DirectX::SimpleMath::Vector3 targetPos,
	float maxDepth
)
{
	// デバイスコンテキストのクリア
	context->ClearState();

	// 深度バッファをクリア
	context->ClearDepthStencilView(m_DSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	// レンダーターゲットをクリア
	context->ClearRenderTargetView(m_shadowMapRTV.Get(), Colors::White);

	// レンダーターゲットを設定
	context->OMSetRenderTargets(1, m_shadowMapRTV.GetAddressOf(), m_DSV.Get());

	// ビューポートの設定
	context->RSSetViewports(1, &m_viewPort);

	//----------------------------------//
	// 定数バッファを設定               //
	//----------------------------------//
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		DX::ThrowIfFailed(
			context->Map(m_shadowMapConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)
		);

		ConstantBuffer buffer = {};

		Vector3 vec = targetPos - lightPos;
		Matrix lightView = Matrix::CreateLookAt(lightPos, targetPos, CalcUpVector(vec));
		Matrix projection = Matrix::CreatePerspectiveFieldOfView(XMConvertToRadians(45.0f), 1.0f, NEAR_CLIP, NEAR_CLIP + maxDepth);
		Matrix lightViewProj = lightView * projection;
		buffer.lightViewProj = XMMatrixTranspose(lightViewProj);
		buffer.lightPosition = lightPos;
		buffer.maxDepth = maxDepth;
		buffer.offset = m_offset / maxDepth;
		//		buffer.VSMFilterEnable = m_vsmFilterEnable;
		buffer.VSMFilterEnable = false;

		*static_cast<ConstantBuffer*>(mappedResource.pData) = buffer;
		context->Unmap(m_shadowMapConstantBuffer.Get(), 0);
	}
	//----------------------------------//
	// 定数バッファを設定（終）         //
	//----------------------------------//
}

// シャドウマップ作成時に呼び出すカスタムステート
void ShadowMap::DrawShadowMap(ID3D11DeviceContext* context)
{
	// 入力レイアウトの設定
	context->IASetInputLayout(m_shadowMapInputLayout.Get());

	// 頂点シェーダーの設定
	context->VSSetShader(m_VS_ShadowMap.Get(), nullptr, 0);
	context->VSSetConstantBuffers(1, 1, m_shadowMapConstantBuffer.GetAddressOf());

	// ピクセルシェーダーの設定
	context->PSSetShader(m_PS_ShadowMap.Get(), nullptr, 0);
	context->PSSetConstantBuffers(1, 1, m_shadowMapConstantBuffer.GetAddressOf());
}

// 影付きのモデルを描画したい時に呼び出すカスタムステート
// テクスチャなしのモデルを表示したい場合は第２引数をfalseにする事
void ShadowMap::DrawShadow(ID3D11DeviceContext* context, bool texture)
{
	// 入力レイアウトの設定
	context->IASetInputLayout(m_shadowMapInputLayout.Get());

	// 頂点シェーダーの設定
	context->VSSetShader(m_VS_ShadowModel.Get(), nullptr, 0);
	context->VSSetConstantBuffers(1, 1, m_shadowMapConstantBuffer.GetAddressOf());

	// ピクセルシェーダーの設定
	if (texture)
	{
		// テクスチャ有
		context->PSSetShader(m_PS_ShadowModel_Texture.Get(), nullptr, 0);
	}
	else
	{
		// テクスチャなし
		context->PSSetShader(m_PS_ShadowModel.Get(), nullptr, 0);
	}
	context->PSSetConstantBuffers(1, 1, m_shadowMapConstantBuffer.GetAddressOf());
	context->PSSetSamplers(1, 1, m_sampler.GetAddressOf());

	// シャドウマップをシェダーリソースビューへ設定する
	context->PSSetShaderResources(1, 1, m_shadowMapSRV.GetAddressOf());
}

// csoの入っているディレクトの設定関数
void ShadowMap::SetDirectory(const wchar_t* path)
{
	if (path)
	{
		m_path = path;
		if (m_path[m_path.size() - 1] != L'/')
		{
			m_path += L"/";
		}
	}
	else
	{
		m_path.clear();
	}
}

// 上方向ベクトルを算出する関数
DirectX::SimpleMath::Vector3 ShadowMap::CalcUpVector(const DirectX::SimpleMath::Vector3& v)
{
	Matrix rotY = Matrix::CreateRotationY(atan2f(v.z, v.x));
	Vector3 tmp = Vector3::Transform(v, rotY);
	Vector3 up = Vector3(-tmp.y, tmp.x, tmp.z);
	up = Vector3::Transform(up, rotY.Invert());
	up.Normalize();
	return up;
}

