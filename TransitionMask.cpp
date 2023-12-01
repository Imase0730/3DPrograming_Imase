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
{
	// スプライトバッチの作成
	m_spriteBatch = std::make_unique<SpriteBatch>(context);
}

// 更新処理
void TransitionMask::Update(float elapsedTime)
{
	if (m_open)
	{
		// オープン
		m_rate -= elapsedTime / m_interval;
		if (m_rate < 0.0f) m_rate = 0.0f;
	}
	else
	{
		// クローズ
		m_rate += elapsedTime / m_interval;
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
	m_spriteBatch->Begin();

	// マスクの描画
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

// オープンしているかチェックする関数
bool TransitionMask::IsOpen()
{
	if (m_open && m_rate == 0.0f) return true;
	return false;
}

// クローズしているかチェックする関数
bool TransitionMask::IsClose()
{
	if (!m_open && m_rate == 1.0f) return true;
	return false;
}
