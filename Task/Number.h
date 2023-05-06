#pragma once

#include "../ImaseLib/TaskManager.h"
#include "../SpriteSheet.h"

// 数字を表示するタスク
class Number : public Imase::Task
{
public:

	// コンストラクタ
	Number(
		std::unique_ptr<DirectX::SpriteBatch>* pSpriteBatch,
		ID3D11ShaderResourceView** pNumberSRV,
		uint32_t digit = 8,	// 桁数（表示位置から何桁で表示するか指定する。右揃えのため）
		float scale = 1.0f
	);

	// 更新
	bool Update(float elapsedTime) override;

	// 描画
	void Render() override;

private:

	// スプライトシート
	std::unique_ptr<SpriteSheet> m_sprites;

	// スプライトバッチへのポインタ
	std::unique_ptr<DirectX::SpriteBatch>* m_pSpriteBatch;

	// 数字のテクスチャハンドルへのポインタ
	ID3D11ShaderResourceView** m_pNumberSRV;

	// 表示位置
	DirectX::SimpleMath::Vector2 m_pos;

	// 表示桁数
	uint32_t m_digit;

	// スケール
	float m_scale;

	// 表示する数字
	uint32_t m_number;

	// ０～９の数字のスプライトフレームへのポインタ
	const SpriteSheet::SpriteFrame* m_frame[10];

public:

	// 表示桁数を設定する関数
	void SetDigit(uint32_t digit) { m_digit = digit; }

	// スケールを設定する関数
	void SetScale(float scale) { m_scale = scale; }

	// 表示する数字を設定する関数
	void SetNumber(uint32_t number) { m_number = number; }

	// 表示位置を設定を設定する関数
	void SetPosition(DirectX::SimpleMath::Vector2 pos) { m_pos = pos; }

};
