#pragma once

#include "ImaseLib/SceneManager.h"
#include "UserResources.h"
#include "ImaseLib/GridFloor.h"
#include "ImaseLib/DebugCamera.h"
#include "ImaseLib/DisplayCollision.h"
#include "Object.h"

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

	// 飛行機モデルへのポインタ
	std::unique_ptr<DirectX::Model> m_planeModel;

private:

	// ベーシックエフェクトへのポインタ
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;

	// プリミティブバッチへのポインタ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;

	// 入力レイアウトへのポインタ
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

private:

	// 飛行機の位置
	DirectX::SimpleMath::Vector3 m_planePos;

	// 飛行機の姿勢を表す回転クォータニオン
	DirectX::SimpleMath::Quaternion m_rotate;

private:

	// 衝突判定の表示オブジェクトへのポインタ
	std::unique_ptr<Imase::DisplayCollision> m_displayCollision;

	// 衝突判定用オブジェクト
	CollisionTest::Object m_object[2];

	// 選択中のオブジェクトの番号
	int m_selectNo;

	// 球と球の衝突判定
	bool HitCheck_Sphere2Sphere(
		const DirectX::BoundingSphere& sphere1,
		const DirectX::BoundingSphere& sphere2
	);
};

