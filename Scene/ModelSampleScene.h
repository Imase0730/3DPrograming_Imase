#pragma once

#include "ImaseLib/SceneManager.h"
#include "UserResources.h"
#include "ImaseLib/GridFloor.h"
#include "ImaseLib/DebugCamera.h"
#include "ImaseLib/DisplayCollision.h"
#include "Object.h"
#include "ImaseLib/ModelCollision.h"
#include "ImaseLib/ObjCollision.h"
#include "GameCamera.h"
#include "ImaseLib/ModelPart.h"

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

	// 車モデルへのポインタ
	std::unique_ptr<DirectX::Model> m_carModel;

	// スカイドームモデルへのポインタ
	std::unique_ptr<DirectX::Model> m_skydomeModel;

	// 戦車モデルへのポインタ
	std::unique_ptr<DirectX::Model> m_tankModel;

	// ボーンモデルへのポインタ
	std::unique_ptr<DirectX::Model> m_boneModel;

private:

	// ロボットの位置
	DirectX::SimpleMath::Vector3 m_robotPosition;

	// ロボットの回転
	DirectX::SimpleMath::Quaternion m_robotRotate;

	// ロボットの上半身の回転
	DirectX::SimpleMath::Quaternion m_bodyRotate;

	// ロボットの左腕の回転
	DirectX::SimpleMath::Quaternion m_armRotate;

	// カメラ
	GameCamera m_camera;

private:

	// ロボットの各パーツモデルへのポインタ
	std::unique_ptr<DirectX::Model> m_headModel;
	std::unique_ptr<DirectX::Model> m_bodyModel;
	std::unique_ptr<DirectX::Model> m_legModel;
	std::unique_ptr<DirectX::Model> m_armRModel;
	std::unique_ptr<DirectX::Model> m_armLModel;
	std::unique_ptr<DirectX::Model> m_missileModel;

	enum { ROOT, HEAD, BODY, LEG, ARM_R, ARM_L, MISSILE, PARTS_CNT };

	// ロボットのパーツへのポインタ
	std::unique_ptr<Imase::ModelPart> m_parts[PARTS_CNT];

	// ミサイル発射フラグ
	bool m_fireFlag;

	// ミサイルの位置
	DirectX::SimpleMath::Vector3 m_missilePosition;

	// ミサイルの回転
	DirectX::SimpleMath::Quaternion m_missileRotate;

	// ミサイルの移動距離
	float m_distance;

private:

	// 床モデルへのポインタ
	std::unique_ptr<DirectX::Model> m_floorModel;

	// 影のテクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shadowTexture;

	// ベーシックエフェクト
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;

	// プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>> m_primitiveBatch;

	// 入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	// 影の初期化関数
	void InitializeShadow(ID3D11Device* device, ID3D11DeviceContext* context);

	// 影の終了処理
	void ResetShadow();

	// 影の描画関数
	void DrawShadow(
		ID3D11DeviceContext* context,
		DirectX::CommonStates* states,
		DirectX::SimpleMath::Vector3 position,
		float radius = 0.5f
	);

	// 深度ステンシルステート（床）
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState_Floor;

	// 深度ステンシルステート（影）
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState_Shadow;

};

