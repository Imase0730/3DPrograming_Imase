#include "pch.h"
#include "ModelSampleScene.h"
#include "DebugDraw.h"

using namespace DirectX;

ModelSampleScene::ModelSampleScene()
	: m_fireFlag(false), m_distance(0.0f)
{
}

void ModelSampleScene::Initialize()
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();

	// デバッグカメラの作成
	RECT rect = GetUserResources()->GetDeviceResources()->GetOutputSize();
	m_debugCamera = std::make_unique<Imase::DebugCamera>(rect.right, rect.bottom);

	// カメラにプレイヤーに位置と回転を渡す
	m_camera.SetPlayer(m_robotPosition, m_robotRotate);

	// ロボットの各パーツの作成
	m_parts[ROOT] = std::make_unique<Imase::ModelPart>();
	m_parts[HEAD] = std::make_unique<Imase::ModelPart>(m_headModel.get());
	m_parts[BODY] = std::make_unique<Imase::ModelPart>(m_bodyModel.get());
	m_parts[LEG] = std::make_unique<Imase::ModelPart>(m_legModel.get());
	m_parts[ARM_R] = std::make_unique<Imase::ModelPart>(m_armRModel.get());
	m_parts[ARM_L] = std::make_unique<Imase::ModelPart>(m_armLModel.get());
	m_parts[MISSILE] = std::make_unique<Imase::ModelPart>(m_missileModel.get());

	// ロボットの各パーツを連結する
	m_parts[ROOT]->SetChild(m_parts[LEG].get());
	m_parts[LEG]->SetChild(m_parts[BODY].get());
	m_parts[BODY]->SetChild(m_parts[HEAD].get());
		m_parts[HEAD]->SetSibling(m_parts[ARM_R].get());
	m_parts[ARM_R]->SetSibling(m_parts[ARM_L].get());
	m_parts[ARM_L]->SetChild(m_parts[MISSILE].get());

	// 各パーツの初期位置行列を作成する
	m_parts[BODY]->SetInitialMatrix(
		SimpleMath::Matrix::CreateTranslation(0.0f, 0.32f, 0.0f));
	m_parts[HEAD]->SetInitialMatrix(
		SimpleMath::Matrix::CreateTranslation(0.0f, 0.75f, 0.0f));
	m_parts[ARM_R]->SetInitialMatrix(
		SimpleMath::Matrix::CreateTranslation(-0.18f, 0.72f, 0.0f));
	m_parts[ARM_L]->SetInitialMatrix(
		SimpleMath::Matrix::CreateTranslation(0.18f, 0.72f, 0.0f));
	m_parts[MISSILE]->SetInitialMatrix(
		SimpleMath::Matrix::CreateTranslation(0.28f, 0.52f, 0.39f));

	// 連結されているパーツの初期位置行列を親の座標系に全て変換する
	m_parts[ROOT]->SetupMatrix();
}

void ModelSampleScene::Update(float elapsedTime)
{
	elapsedTime;

	// デバッグカメラの更新
	m_debugCamera->Update();

	auto kb = Keyboard::Get().GetState();
	auto kbTracker = GetUserResources()->GetKeyboardStateTracker();

	// ----- カメラの切り替え ----- //

	if (kbTracker->pressed.Z)
	{
		m_camera.SetType(GameCamera::Type::Type_A);
	}

	if (kbTracker->pressed.X)
	{
		m_camera.SetType(GameCamera::Type::Type_B);
	}

	// ----- ロボットの移動 ----- //

	// 前進
	if (kb.A)
	{
		m_robotRotate *= SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::UnitY, XMConvertToRadians(1.0f));
	}
	// 後進
	if (kb.D)
	{
		m_robotRotate *= SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::UnitY, XMConvertToRadians(-1.0f));
	}
	// 右旋回
	if (kb.W)
	{
		m_robotPosition += SimpleMath::Vector3::Transform(SimpleMath::Vector3(0.0f, 0.0f, 0.02f), m_robotRotate);
	}
	// 左旋回
	if (kb.S)
	{
		m_robotPosition += SimpleMath::Vector3::Transform(SimpleMath::Vector3(0.0f, 0.0f, -0.02f), m_robotRotate);
	}

	// ----- ロボットの上半身の回転 ----- //
	if (kb.Left)
	{
		m_bodyRotate *= SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::UnitY, XMConvertToRadians(1.0f));
	}
	if (kb.Right)
	{
		m_bodyRotate *= SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::UnitY, XMConvertToRadians(-1.0f));
	}

	// ----- ロボットの左腕の回転 ----- //
	if (kb.Up)
	{
		m_armRotate *= SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::UnitX, XMConvertToRadians(1.0f));
	}
	if (kb.Down)
	{
		m_armRotate *= SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::UnitX, XMConvertToRadians(-1.0f));
	}

	// スペースキーでミサイルを発射
	if (!m_fireFlag && kbTracker->pressed.Space)
	{
		m_fireFlag = true;
		// ミサイルを左腕から外す
		m_parts[ARM_L]->SetChild(nullptr);
		// ミサイルのワールド行列を取得
		SimpleMath::Matrix m = m_parts[MISSILE]->GetWorldMatrix();
		// ミサイルの位置を取得
		m_missilePosition.x = m._41;
		m_missilePosition.y = m._42;
		m_missilePosition.z = m._43;
		// ミサイルの回転を取得
		m_missileRotate = SimpleMath::Quaternion::CreateFromRotationMatrix(m);
	}

	// ミサイル発射中なら
	if (m_fireFlag)
	{
		// ミサイルを回転させる
		m_missileRotate = SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::UnitZ, XMConvertToRadians(5.0f))
						* m_missileRotate;
		// ミサイルを向いている方向へ移動させる
		SimpleMath::Vector3 v(0.0f, 0.0f, 0.02f);
		m_missilePosition += SimpleMath::Vector3::Transform(v, m_missileRotate);
		m_distance += v.Length();
		// 移動距離が３ｍを超えたら腕にミサイルを戻す
		if (m_distance > 3.0f)
		{
			m_fireFlag = false;
			m_distance = 0.0f;
			m_parts[ARM_L]->SetChild(m_parts[MISSILE].get());
		}
	}

	// カメラの更新
	m_camera.Update(elapsedTime);
}

void ModelSampleScene::Render()
{
	auto debugFont = GetUserResources()->GetDebugFont();
	debugFont->AddString(L"ModelSampleScene", SimpleMath::Vector2(0.0f, debugFont->GetFontHeight()));

	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
	auto states = GetUserResources()->GetCommonStates();

	// ビュー行列を設定
	//m_view = m_debugCamera->GetCameraMatrix();
	m_view = SimpleMath::Matrix::CreateLookAt(
		m_camera.GetEyePosition(),
		m_camera.GetTargetPosition(),
		SimpleMath::Vector3::UnitY
	);

	// グリッドの床を描画
	m_gridFloor->Render(context, m_view, m_proj);

	// ロボットの上半身の回転
	m_parts[BODY]->SetTransformMatrix(SimpleMath::Matrix::CreateFromQuaternion(m_bodyRotate));

	// ロボットの左腕の回転
	m_parts[ARM_L]->SetTransformMatrix(SimpleMath::Matrix::CreateFromQuaternion(m_armRotate));

	// ロボットを移動させる
	SimpleMath::Matrix m = SimpleMath::Matrix::CreateFromQuaternion(m_robotRotate)
		                 * SimpleMath::Matrix::CreateTranslation(m_robotPosition);
	m_parts[ROOT]->SetTransformMatrix(m);

	// ロボットの描画
	m_parts[ROOT]->UpdateMatrix();
	m_parts[ROOT]->Draw(context, *states, m_view, m_proj, [&]()
		{
			// カリングしない
			context->RSSetState(states->CullNone());
		}
	);

	// ミサイル発射中なら
	if (m_fireFlag)
	{
		// ミサイルを描画する
		SimpleMath::Matrix world = SimpleMath::Matrix::CreateFromQuaternion(m_missileRotate)
								 * SimpleMath::Matrix::CreateTranslation(m_missilePosition);
		m_parts[MISSILE]->GetModel()->Draw(context, *states, world, m_view, m_proj);
	}

}

void ModelSampleScene::Finalize()
{
	m_gridFloor.reset();

	m_headModel.reset();
	m_bodyModel.reset();
	m_legModel.reset();
	m_armRModel.reset();
	m_armLModel.reset();
	m_missileModel.reset();
}

void ModelSampleScene::CreateDeviceDependentResources()
{
	auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();
	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
	auto states = GetUserResources()->GetCommonStates();

	// グリッドの床を作成
	m_gridFloor = std::make_unique<Imase::GridFloor>(device, context, states);

	// エフェクトファクトリーの作成
	std::unique_ptr<EffectFactory> fx = std::make_unique<EffectFactory>(device);
	fx->SetDirectory(L"Resources/Models");

	// ロボットの各パーツモデルの作成
	m_headModel = Model::CreateFromCMO(device, L"Resources/Models/Head.cmo", *fx);
	m_bodyModel = Model::CreateFromCMO(device, L"Resources/Models/Body.cmo", *fx);
	m_legModel = Model::CreateFromCMO(device, L"Resources/Models/Leg.cmo", *fx);
	m_armRModel = Model::CreateFromCMO(device, L"Resources/Models/Arm_R.cmo", *fx);
	m_armLModel = Model::CreateFromCMO(device, L"Resources/Models/Arm_L.cmo", *fx);
	m_missileModel = Model::CreateFromCMO(device, L"Resources/Models/Missile.cmo", *fx);
}

void ModelSampleScene::CreateWindowSizeDependentResources()
{
	// 射影行列を作成
	RECT rect = GetUserResources()->GetDeviceResources()->GetOutputSize();
	m_proj = SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		XMConvertToRadians(45.0f),
		static_cast<float>(rect.right) / static_cast<float>(rect.bottom),
		0.1f, 200.0f
	);
}

void ModelSampleScene::OnDeviceLost()
{
	Finalize();
}

