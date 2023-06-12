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

	// 飛行機の初期姿勢の設定
	m_rotate = SimpleMath::Quaternion::FromToRotation(SimpleMath::Vector3(0.0f, 0.0f, 1.0f), SimpleMath::Vector3(0.0f, 0.0f, -1.0f));

	// 衝突判定用オブジェクトの初期値を設定する
	m_object[0].position = SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
	m_object[1].position = SimpleMath::Vector3(-1.0f, 0.0f, 0.0f);

	// 衝突判定オブジェクトを作成する
	m_modelCollision[0] = Imase::ModelCollisionFactory::CreateCollision(Imase::ModelCollision::CollisionType::OBB, m_pacmanModel.get());
	m_modelCollision[1] = Imase::ModelCollisionFactory::CreateCollision(Imase::ModelCollision::CollisionType::Sphere, m_planeModel.get());
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

	// コリジョン情報の更新
	m_modelCollision[0]->UpdateBoundingInfo(m_object[0].position, m_object[0].rotate);
	m_modelCollision[1]->UpdateBoundingInfo(m_object[1].position, m_object[1].rotate);

}

void ModelSampleScene::Render()
{
	auto debugFont = GetUserResources()->GetDebugFont();
	debugFont->AddString(L"ModelSampleScene", SimpleMath::Vector2(0.0f, debugFont->GetFontHeight()));

	// オブジェクト同士の衝突判定を行う
	if (m_modelCollision[0]->Intersects(m_modelCollision[1].get()))
	{
		debugFont->AddString(L"Hit!", SimpleMath::Vector2(0.0f, debugFont->GetFontHeight() * 2));
	}

	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
	auto states = GetUserResources()->GetCommonStates();

	// ビュー行列を設定
	m_view = m_debugCamera->GetCameraMatrix();

	// グリッドの床を描画
	m_gridFloor->Render(context, m_view, m_proj);

	// パックマンの描画
	SimpleMath::Matrix rotate = SimpleMath::Matrix::CreateFromQuaternion(m_object[0].rotate);
	SimpleMath::Matrix trans = SimpleMath::Matrix::CreateTranslation(m_object[0].position);
	SimpleMath::Matrix world = rotate * trans;
	m_pacmanModel->Draw(context, *states, world, m_view, m_proj);

	// 飛行機の描画
	rotate = SimpleMath::Matrix::CreateFromQuaternion(m_object[1].rotate);
	trans = SimpleMath::Matrix::CreateTranslation(m_object[1].position);
	world = rotate * trans;
	m_planeModel->Draw(context, *states, world, m_view, m_proj);

	// 軸の描画
	// 選択中のオブジェクトを取得する
	CollisionTest::Object* p = &m_object[m_selectNo];

	// 角軸のベクトルを求める
	SimpleMath::Vector3 forward = SimpleMath::Vector3::Transform(SimpleMath::Vector3(0.0f, 0.0f, 1.5f), p->rotate);
	SimpleMath::Vector3 horizontal = SimpleMath::Vector3::Transform(SimpleMath::Vector3(1.5f, 0.0f, 0.0f), p->rotate);
	SimpleMath::Vector3 vertical = SimpleMath::Vector3::Transform(SimpleMath::Vector3(0.0f, 1.5f, 0.0f), p->rotate);

	context->OMSetBlendState(states->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(states->DepthDefault(), 0);
	context->RSSetState(states->CullNone());

	m_basicEffect->SetView(m_view);
	m_basicEffect->SetProjection(m_proj);
	m_basicEffect->Apply(context);

	context->IASetInputLayout(m_inputLayout.Get());

	m_primitiveBatch->Begin();

	DX::DrawRay(m_primitiveBatch.get(), p->position, forward, false, Colors::Yellow);
	DX::DrawRay(m_primitiveBatch.get(), p->position, horizontal, false, Colors::Red);
	DX::DrawRay(m_primitiveBatch.get(), p->position, vertical, false, Colors::Blue);

	m_primitiveBatch->End();

	// 衝突判定の登録
	m_modelCollision[0]->AddDisplayCollision(m_displayCollision.get());
	m_modelCollision[1]->AddDisplayCollision(m_displayCollision.get());

	// 衝突判定の表示
	m_displayCollision->DrawCollision(context, states, m_view, m_proj);
}

void ModelSampleScene::Finalize()
{
	m_basicEffect.reset();
	m_primitiveBatch.reset();
	m_inputLayout.Reset();
	m_gridFloor.reset();
	m_planeModel.reset();
	m_displayCollision.reset();
}

void ModelSampleScene::CreateDeviceDependentResources()
{
	auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();
	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
	auto states = GetUserResources()->GetCommonStates();

	// ベーシックエフェクトの作成
	m_basicEffect = std::make_unique<BasicEffect>(device);
	m_basicEffect->SetVertexColorEnabled(true);

	// プリミティブバッチの作成
	m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);

	// 入力レイアウトの作成
	DX::ThrowIfFailed(
		CreateInputLayoutFromEffect<VertexPositionColor>(
			device,
			m_basicEffect.get(),
			m_inputLayout.ReleaseAndGetAddressOf())
	);

	// グリッドの床を作成
	m_gridFloor = std::make_unique<Imase::GridFloor>(device, context, states);

	// 飛行機モデル作成
	std::unique_ptr<EffectFactory> fx = std::make_unique<EffectFactory>(device);
	fx->SetDirectory(L"Resources/Models");
	m_planeModel = Model::CreateFromCMO(device, L"Resources/Models/Plane.cmo", *fx);

	// パックマンモデルの作成
	m_pacmanModel = Model::CreateFromCMO(device, L"Resources/Models/Pacman.cmo", *fx);

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

// 球と球の衝突判定関数
bool ModelSampleScene::HitCheck_Sphere2Sphere(
	const DirectX::BoundingSphere& sphere1,
	const DirectX::BoundingSphere& sphere2
)
{
	// 2つの球の中心の間の距離の平方を計算
	SimpleMath::Vector3 d = SimpleMath::Vector3(sphere1.Center) - SimpleMath::Vector3(sphere2.Center);
	float distSq = d.Dot(d);
	// 平方した距離が平方した半径の合計よりも小さい場合に球は交差している
	float radiusSum = sphere1.Radius + sphere2.Radius;
	return distSq <= radiusSum * radiusSum;
}
