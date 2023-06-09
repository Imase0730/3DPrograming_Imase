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

	// 衝突判定の表示オブジェクトへのポインタ
	std::unique_ptr<Imase::DisplayCollision> m_displayCollision;

	// 衝突判定用オブジェクト
	CollisionTest::Object m_object[2];

	// 選択中のオブジェクトの番号
	int m_selectNo;

	// 衝突判定用メッシュへのポインタ
	std::unique_ptr<Imase::ObjCollision> m_objCollision;

	// 線分
	DirectX::SimpleMath::Vector3 m_line[2];


private:

	// 戦車の位置
	DirectX::SimpleMath::Vector3 m_tankPosition;

	// 戦車の回転
	DirectX::SimpleMath::Quaternion m_tankRotate;

	// カメラ
	GameCamera m_camera;

private:

	enum { PARENT, CHILD };

	// ボーンの数
	static const int BONE_CNT = 2;

	// 各ボーンの初期位置行列
	DirectX::SimpleMath::Matrix m_initialMatrix[BONE_CNT];

	// 各ボーンの変換行列
	DirectX::SimpleMath::Matrix m_transformMatrix[BONE_CNT];

private:

	// ロボットの各パーツモデルへのポインタ
	std::unique_ptr<DirectX::Model> m_headModel;
	std::unique_ptr<DirectX::Model> m_bodyModel;
	std::unique_ptr<DirectX::Model> m_legModel;
	std::unique_ptr<DirectX::Model> m_armRModel;
	std::unique_ptr<DirectX::Model> m_armLModel;

	enum { ROOT, HEAD, BODY, LEG, ARM_R, ARM_L, PARTS_CNT };

	// ロボットのパーツへのポインタ
	std::unique_ptr<Imase::ModelPart> m_parts[PARTS_CNT];

};

