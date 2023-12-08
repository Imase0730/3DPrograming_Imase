//--------------------------------------------------------------------------------------
// File: TransitionMask.cpp
//
// 画面切り替え用のマスク表示クラス
//
// Date: 2023.9.10
// Author: Hideyasu Imase
//--------------------------------------------------------------------------------------
#include "pch.h"
#include "TransitionMask.h"
#include "ReadData.h"
#include "CommonSys.h"

using namespace DirectX;

// コンストラクタ
TransitionMask::TransitionMask(
	ID3D11Device* device,
	ID3D11DeviceContext* context,
	float interval
)
	: m_interval(interval)
	, m_rate(0.0f)
	, m_open(true)
	, m_request(CreateMaskRequest::NONE)
{
	// スプライトバッチの作成
	m_spriteBatch = std::make_unique<SpriteBatch>(context);

	// ピクセルシェーダーの作成
	std::vector<uint8_t> ps_fade = DX::ReadData(L"Resources/Shaders/PS_Fade.cso");
	DX::ThrowIfFailed(
		device->CreatePixelShader(ps_fade.data(), ps_fade.size(), nullptr, m_PS_Fade.ReleaseAndGetAddressOf())
	);

	// 定数バッファの作成
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(ConstantBuffer));	// 確保するサイズ（16の倍数で設定する）
	// GPU (読み取り専用) と CPU (書き込み専用) の両方からアクセスできるリソース
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// 定数バッファとして扱う
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPUが内容を変更できるようにする
	DX::ThrowIfFailed(device->CreateBuffer(&bufferDesc, nullptr, m_constantBuffer.ReleaseAndGetAddressOf()));

	// マスク用テクスチャの読み込み
	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"Resources/Textures/FadeMask.dds", nullptr, m_maskTexture.ReleaseAndGetAddressOf())
	);
}

// 更新処理
void TransitionMask::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	if (m_open)
	{
		// オープン
		m_rate -= FRAME_TIME / m_interval;
		if (m_rate < 0.0f) m_rate = 0.0f;
	}
	else
	{
		// クローズ
		m_rate += FRAME_TIME / m_interval;
		if (m_rate > 1.0f) m_rate = 1.0f;
	}
}

// 描画処理
void TransitionMask::Draw(
	ID3D11DeviceContext* context,
	DirectX::CommonStates* states,
	ID3D11ShaderResourceView* texture,
	const RECT& rect
)
{
	if (m_rate == 0.0f) return;

	// -------------------------------------------------------------------------- //
	// 定数バッファを更新
	// -------------------------------------------------------------------------- //

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// GPUが定数バッファに対してアクセスを行わないようにする
	context->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// 定数バッファを更新
	static_cast<ConstantBuffer*>(mappedResource.pData)->rate = m_rate;

	// GPUが定数バッファに対してのアクセスを許可する
	context->Unmap(m_constantBuffer.Get(), 0);

	m_spriteBatch->Begin(SpriteSortMode_Immediate, states->NonPremultiplied(), nullptr, nullptr, nullptr, [&]()
		{
			// 定数バッファの設定
			ID3D11Buffer* cbuf[] = { m_constantBuffer.Get() };
			context->PSSetConstantBuffers(1, 1, cbuf);

			// マスク用テクスチャを設定
			auto srv = m_maskTexture.Get();
			context->PSSetShaderResources(1, 1, &srv);

			// シェーダーの設定
			context->PSSetShader(m_PS_Fade.Get(), nullptr, 0);
		}
	);

	m_spriteBatch->Draw(texture, rect);

	m_spriteBatch->End();
}

// マスクをオープンする関数
void TransitionMask::Open()
{
	m_open = true;
	m_rate = 1.0f;
}

// マスクをクローズする関数
void TransitionMask::Close()
{
	m_open = false;
	m_rate = 0.0f;
}

