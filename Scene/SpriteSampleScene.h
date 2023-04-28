#pragma once

#include "../ImaseLib/SceneManager.h"
#include "../UserResources.h"
#include "../ImaseLib/TaskManager.h"

class SpriteSampleScene : public Imase::Scene<UserResources>
{
public:
	
	// コンストラクタ
	SpriteSampleScene();

	// 初期化
	void Initialize() override;

	// 更新
	void Update(float elapsedTime) override;

	// 描画
	void Render() override;

	// 終了処理
	void Finalize() override;

	// デバイスに依存するリソースを作成する関数
	void CreateDeviceDependentResources() override;

	// ウインドウサイズに依存するリソースを作成する関数
	void CreateWindowSizeDependentResources() override;

	// デバイスロストした時に呼び出される関数
	void OnDeviceLost() override;

private:

	// スプライトバッチへのポインタ
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

	// 猫の絵のテクスチャハンドル
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_catSRV;

	// ロボットの絵のテクスチャハンドル
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_robotSRV;

	// カードの絵のテクスチャハンドル
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cardSRV[3];

	// ばね係数
	static const float K;

	// 摩擦係数
	static const float FRICTION;

	// ロボットの絵のスケール
	float m_robotScale;

	// ロボットの絵のスケールの速度
	float m_robotVelocity;

	// タスクマネージャー
	Imase::TaskManager m_taskManager;

};

