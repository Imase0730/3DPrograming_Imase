//--------------------------------------------------------------------------------------
// File: TransitionMask.h
//
// 画面切り替え用のマスク表示クラス
//
// Date: 2023.9.10
// Author: Hideyasu Imase
//--------------------------------------------------------------------------------------
#pragma once

class TransitionMask
{
private:

	// 割合(0～1)
	float m_rate;

	// オープン又はクローズするまでの時間
	float m_interval;

	// オープンフラグ（trueの場合オープン）
	bool m_open;

	// スプライトバッチ
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

public:

	// コンストラクタ
	TransitionMask(
		ID3D11Device* device,
		ID3D11DeviceContext* context,
		float interval
	);

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Draw(
		ID3D11DeviceContext* context,
		DirectX::CommonStates* states,
		ID3D11ShaderResourceView* texture,
		const RECT& rect
	);

	// オープンする関数
	void Open();

	// クローズする関数
	void Close();

	// オープン中かチェックする関数
	bool IsOpen();

	// クローズ中かチェックする関数
	bool IsClose();

	// オープン具合を返す関数(0～1)
	float GetOpenRate() { return m_rate; }

	// オープン、クローズまでの時間を設定する関数
	void SetInterval(float interval) { m_interval = interval; }

};
