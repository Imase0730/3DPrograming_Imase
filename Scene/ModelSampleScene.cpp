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

	// 衝突判定用オブジェクトの初期値を設定する
	m_object[0].position = SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
	m_object[1].position = SimpleMath::Vector3(-1.0f, 0.0f, 0.0f);

	// 衝突判定用メッシュオブジェクトを作成する
	m_objCollision = std::make_unique<Imase::ObjCollision>(L"Resources/CollisionMeshes/Floor.obj");

	// 線分
	m_line[0] = SimpleMath::Vector3(0, 1, 0);
	m_line[1] = SimpleMath::Vector3(0, -1, 0);
}

void ModelSampleScene::Update(float elapsedTime)
{
	elapsedTime;

	// デバッグカメラの更新
	m_debugCamera->Update();

	auto kb = Keyboard::Get().GetState();
	auto kbTracker = GetUserResources()->GetKeyboardStateTracker();

	// スペースキーが押されたら選択オブジェクトを変更する
	if (kbTracker->pressed.Space)
	{
		if (m_selectNo)
		{
			m_selectNo = 0;
		}
		else
		{
			m_selectNo = 1;
		}
	}

	// 選択中のオブジェクトを取得する
	CollisionTest::Object* p = &m_object[m_selectNo];

	SimpleMath::Quaternion q;

	// ピッチ
	if (kb.W) q *= SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::UnitX, XMConvertToRadians(1.0f));
	if (kb.S) q *= SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::UnitX, XMConvertToRadians(-1.0f));

	// ヨー
	if (kb.Left) q *= SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::UnitY, XMConvertToRadians(1.0f));
	if (kb.Right) q *= SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::UnitY, XMConvertToRadians(-1.0f));

	// ロール
	if (kb.D) q *= SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::UnitZ, XMConvertToRadians(1.0f));
	if (kb.A) q *= SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::UnitZ, XMConvertToRadians(-1.0f));

	// 姿勢に回転を加える
	p->rotate = q * p->rotate;

	// 前進・後進
	if (kb.Up) p->position += SimpleMath::Vector3::Transform(SimpleMath::Vector3(0.0f, 0.0f, 0.1f), p->rotate);
	if (kb.Down) p->position += SimpleMath::Vector3::Transform(-SimpleMath::Vector3(0.0f, 0.0f, 0.1f), p->rotate);

	// 線分の移動
	m_line[0].x = p->position.x;
	m_line[1].x = p->position.x;
	m_line[0].z = p->position.z;
	m_line[1].z = p->position.z;
}

void ModelSampleScene::Render()
{
	auto debugFont = GetUserResources()->GetDebugFont();
	debugFont->AddString(L"ModelSampleScene", SimpleMath::Vector2(0.0f, debugFont->GetFontHeight()));

	// オブジェクト同士の衝突判定を行う
	SimpleMath::Vector3 hitPosition;
	SimpleMath::Vector3 normal;
	SimpleMath::Matrix tilt;
	if (m_objCollision->IntersectLineSegment(m_line[0], m_line[1], &hitPosition, &normal))
	{
		SimpleMath::Quaternion q = SimpleMath::Quaternion::FromToRotation(SimpleMath::Vector3::UnitY, normal);
		tilt = SimpleMath::Matrix::CreateFromQuaternion(q);
		debugFont->AddString(L"Hit!", SimpleMath::Vector2(0.0f, debugFont->GetFontHeight() * 2));
	}

	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
	auto states = GetUserResources()->GetCommonStates();

	// ビュー行列を設定
	m_view = m_debugCamera->GetCameraMatrix();

	// グリッドの床を描画
	//m_gridFloor->Render(context, m_view, m_proj);

	SimpleMath::Matrix rotate, trans, world;

	// 車の描画
	rotate = SimpleMath::Matrix::CreateFromQuaternion(m_object[0].rotate);
	trans = SimpleMath::Matrix::CreateTranslation(hitPosition);
	world = rotate * tilt * trans;
	m_carModel->Draw(context, *states, world, m_view, m_proj);

	// 軸の描画
	// 選択中のオブジェクトを取得する
	CollisionTest::Object* p = &m_object[m_selectNo];

	// 角軸のベクトルを求める
	SimpleMath::Vector3 forward = SimpleMath::Vector3::Transform(SimpleMath::Vector3(0.0f, 0.0f, 1.5f), p->rotate);
	SimpleMath::Vector3 horizontal = SimpleMath::Vector3::Transform(SimpleMath::Vector3(1.5f, 0.0f, 0.0f), p->rotate);
//	SimpleMath::Vector3 vertical = SimpleMath::Vector3::Transform(SimpleMath::Vector3(0.0f, 1.5f, 0.0f), p->rotate);

	m_displayCollision->AddLineSegment(p->position, p->position + forward, Colors::Yellow);
	m_displayCollision->AddLineSegment(p->position, p->position + horizontal, Colors::Red);
	m_displayCollision->AddLineSegment(m_line[0], m_line[1], Colors::Blue);

	// 衝突判定の登録
	m_objCollision->AddDisplayCollision(m_displayCollision.get());

	// 衝突判定の表示
	m_displayCollision->DrawCollision(context, states, m_view, m_proj);
}

void ModelSampleScene::Finalize()
{
	m_gridFloor.reset();
	m_carModel.reset();
	m_displayCollision.reset();
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

	// 車モデルの作成
	m_carModel = Model::CreateFromCMO(device, L"Resources/Models/Car.cmo", *fx);

	// 衝突判定の表示オブジェクトの作成
	m_displayCollision = std::make_unique<Imase::DisplayCollision>(device, context);
}

void ModelSampleScene::CreateWindowSizeDependentResources()
{
	// 射影行列を作成
	RECT rect = GetUserResources()->GetDeviceResources()->GetOutputSize();
	m_proj = SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		XMConvertToRadians(45.0f),
		static_cast<float>(rect.right) / static_cast<float>(rect.bottom),
		0.1f, 100.0f
	);
}

void ModelSampleScene::OnDeviceLost()
{
	Finalize();
}

