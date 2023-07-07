#include "pch.h"
#include "ModelSampleScene.h"
#include "DebugDraw.h"

using namespace DirectX;

ModelSampleScene::ModelSampleScene()
	: m_selectNo(0)
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
	m_camera.SetPlayer(m_tankPosition, m_tankRotate);

	// 各ボーンの初期位置行列を設定
	m_initialMatrix[PARENT] = SimpleMath::Matrix::Identity;
	m_initialMatrix[CHILD] = SimpleMath::Matrix::CreateTranslation(0.0f, 1.0f, 0.0f);

}

void ModelSampleScene::Update(float elapsedTime)
{
	elapsedTime;

	// デバッグカメラの更新
	m_debugCamera->Update();

	auto kb = Keyboard::Get().GetState();
	auto kbTracker = GetUserResources()->GetKeyboardStateTracker();

	// ----- 戦車の移動 ----- //

	// 前進
	if (kb.A)
	{
		m_tankRotate *= SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::UnitY, XMConvertToRadians(1.0f));
	}
	// 後進
	if (kb.D)
	{
		m_tankRotate *= SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::UnitY, XMConvertToRadians(-1.0f));
	}
	// 右旋回
	if (kb.W)
	{
		m_tankPosition += SimpleMath::Vector3::Transform(SimpleMath::Vector3(0.0f, 0.0f, 0.02f), m_tankRotate);
	}
	// 左旋回
	if (kb.S)
	{
		m_tankPosition += SimpleMath::Vector3::Transform(SimpleMath::Vector3(0.0f, 0.0f, -0.02f), m_tankRotate);
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
	m_view = m_debugCamera->GetCameraMatrix();
	//m_view = SimpleMath::Matrix::CreateLookAt(
	//	m_camera.GetEyePosition(),
	//	m_camera.GetTargetPosition(),
	//	SimpleMath::Vector3::UnitY
	//);

	// グリッドの床を描画
	m_gridFloor->Render(context, m_view, m_proj);

	//// 戦車の描画
	//SimpleMath::Matrix rotate = SimpleMath::Matrix::CreateFromQuaternion(m_tankRotate);
	//SimpleMath::Matrix trans = SimpleMath::Matrix::CreateTranslation(m_tankPosition);
	//SimpleMath::Matrix world = rotate * trans;
	//m_tankModel->Draw(context, *states, world, m_view, m_proj, false,[&]()
	//	{
	//		context->RSSetState(states->CullNone());
	//	}
	//);

	auto timer = GetUserResources()->GetStepTimer();

	// ボーンのボーン行列にX軸の回転を加えて回転させる
	float angle = static_cast<float>(timer->GetTotalSeconds());
	m_transformMatrix[PARENT] = SimpleMath::Matrix::CreateRotationX(XM_PIDIV2 * sinf(angle));
	m_transformMatrix[CHILD] = SimpleMath::Matrix::CreateRotationX(XM_PIDIV2 * sinf(angle));

	SimpleMath::Matrix world[BONE_CNT];

	// 各ワールド行列を作成（各ボーンの回転や移動×ボーンの初期位置）
	world[PARENT] = m_transformMatrix[PARENT] * m_initialMatrix[PARENT];
	world[CHILD] = m_transformMatrix[CHILD] * m_initialMatrix[CHILD];

	// 子の行列に親の行列を掛ける
	world[CHILD] = world[CHILD] * world[PARENT];

	// ボーンを描画
	for (int i = 0; i < BONE_CNT; i++)
	{
		m_boneModel->Draw(context, *states, world[i], m_view, m_proj);
	}

}

void ModelSampleScene::Finalize()
{
	m_gridFloor.reset();
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

	// 戦車モデルの作成
	//m_tankModel = Model::CreateFromCMO(device, L"Resources/Models/Tank.cmo", *fx);

	// ボーンモデルの作成
	m_boneModel = Model::CreateFromCMO(device, L"Resources/Models/Bone.cmo", *fx);
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

