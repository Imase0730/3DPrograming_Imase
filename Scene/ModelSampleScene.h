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
#include "RenderTexture.h"

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

private:

	// 床モデルへのポインタ
	std::unique_ptr<DirectX::Model> m_floorModel;

private:

	// トーラスのモデル
	std::unique_ptr<DirectX::Model> m_torusModel;

	// ピクセルシェーダー（トーラス用）
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PS_Torus;

private:

	// 定数バッファの構造体
	struct ConstantBuffer
	{
		float att0;		// 一定減衰係数
		float att1;		// 線形減衰係数
		float att2;		// 次減衰係数
		float padding1;
		DirectX::XMVECTOR lightPosition;	// ライトの位置
	};

	// 減衰係数
	float m_att[3];

	// ライトの位置
	DirectX::SimpleMath::Vector3 m_lightPosition;

	// 定数バッファへのポインタ
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;

	// 減衰係数デバッグ表示のカーソル位置
	int m_cursor;

private:

	// シーンの描画
	void DrawScene(const D3D11_VIEWPORT& vp, const DirectX::SimpleMath::Matrix& view);

private:

	// レンダーテクスチャ(シーン全体)
	std::unique_ptr<DX::RenderTexture> m_sceneRT;

	// スプライトバッチ
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

	// ベーシックポストプロセス
	std::unique_ptr<DirectX::BasicPostProcess> m_basicPostProcess;

};

