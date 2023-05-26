#pragma once

#include "ImaseLib/SceneManager.h"
#include "UserResources.h"
#include "ImaseLib/GridFloor.h"
#include "ImaseLib/DebugCamera.h"

class ModelSampleScene : public Imase::Scene<UserResources>
{
public:
	
	// コンストラクタ
	ModelSampleScene();

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

	// デバッグカメラへのポインタ
	std::unique_ptr<Imase::DebugCamera> m_debugCamera;

	// ビュー行列
	DirectX::SimpleMath::Matrix m_view;

	// 射影行列
	DirectX::SimpleMath::Matrix m_proj;

	// グリッドの床へのポインタ
	std::unique_ptr<Imase::GridFloor> m_gridFloor;

	// サイコロモデルへのポインタ
	std::unique_ptr<DirectX::Model> m_diceModel;

	// ボールへのポインタ
	std::unique_ptr<DirectX::Model> m_ballModel;

private:

	// ベーシックエフェクトへのポインタ
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;

	// プリミティブバッチへのポインタ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;

	// 入力レイアウトへのポインタ
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	// ボールの進行方向
	DirectX::SimpleMath::Vector3 m_forward;

	// ボールの回転軸
	DirectX::SimpleMath::Vector3 m_horizontal;

	// ボールの位置
	DirectX::SimpleMath::Vector3 m_pos;

	// ボールの回転
	DirectX::SimpleMath::Quaternion m_rotate;

	// ボールの速さ
	float m_speed;

};

