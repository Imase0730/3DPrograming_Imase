#include "pch.h"
#include "ModelSampleScene.h"
#include "DebugDraw.h"

using namespace DirectX;

ModelSampleScene::ModelSampleScene()
	: m_speed(0.0f)
{
}

void ModelSampleScene::Initialize()
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();

	// デバッグカメラの作成
	RECT rect = GetUserResources()->GetDeviceResources()->GetOutputSize();
	m_debugCamera = std::make_unique<Imase::DebugCamera>(rect.right, rect.bottom);

	// ボールの進行方向を初期化する
	m_forward = SimpleMath::Vector3(0.0f, 0.0f, -1.0f);
}

void ModelSampleScene::Update(float elapsedTime)
{
	elapsedTime;

	// デバッグカメラの更新
	m_debugCamera->Update();

	auto kb = Keyboard::Get().GetState();

	// 矢印キーの左右キーで進行方向を回転させる
	if (kb.Right)
	{
		SimpleMath::Matrix rotY = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(-1.0f));
		m_forward = SimpleMath::Vector3::Transform(m_forward, rotY);
	}
	if (kb.Left)
	{
		SimpleMath::Matrix rotY = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(1.0f));
		m_forward = SimpleMath::Vector3::Transform(m_forward, rotY);
	}

	// 矢印キー上下で加速させる
	if (kb.Up) m_speed += 0.001f;
	if (kb.Down) m_speed -= 0.001f;

	// ボールの回転の軸ベクトルを求める
	m_horizontal = SimpleMath::Vector3(m_forward.z, 0.0f, -m_forward.x);

	// 摩擦係数を掛けて減速させる
	m_speed *= 0.98f;

	// 速度ベクトル
	SimpleMath::Vector3 v = m_forward * m_speed;

	// ボールの移動
	m_pos += v;

	// ボールを回転させる（ボールの半径は0.5なのでボールの移動距離を２倍して求める）
	float angle = v.Length() * 2.0f;
	if (m_speed < 0.0f) angle *= -1.0f;
	m_rotate *= SimpleMath::Quaternion::CreateFromAxisAngle(m_horizontal, angle);

}

void ModelSampleScene::Render()
{
	auto debugFont = GetUserResources()->GetDebugFont();
	debugFont->AddString(L"ModelSampleScene", SimpleMath::Vector2(0.0f, debugFont->GetFontHeight()));

	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
	auto states = GetUserResources()->GetCommonStates();

	// ビュー行列を設定
	m_view = m_debugCamera->GetCameraMatrix();

	// グリッドの床を描画
	m_gridFloor->Render(context, m_view, m_proj);

	// ボールの描画
	SimpleMath::Matrix rotate = SimpleMath::Matrix::CreateFromQuaternion(m_rotate);
	SimpleMath::Matrix transA = SimpleMath::Matrix::CreateTranslation(SimpleMath::Vector3(0.0f, 0.5f, 0.0f));
	SimpleMath::Matrix transB = SimpleMath::Matrix::CreateTranslation(m_pos);
	SimpleMath::Matrix world = rotate * transA * transB;
	m_ballModel->Draw(context, *states, world, m_view, m_proj);

	// 軸の描画
	context->OMSetBlendState(states->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(states->DepthDefault(), 0);
	context->RSSetState(states->CullNone());

	m_basicEffect->SetView(m_view);
	m_basicEffect->SetProjection(m_proj);
	m_basicEffect->Apply(context);

	context->IASetInputLayout(m_inputLayout.Get());

	m_primitiveBatch->Begin();
	DX::DrawRay(m_primitiveBatch.get(), m_pos, m_forward, true, Colors::Cyan);
	DX::DrawRay(m_primitiveBatch.get(), m_pos, m_horizontal, true, Colors::Red);
	m_primitiveBatch->End();
}

void ModelSampleScene::Finalize()
{
	m_gridFloor.reset();
	m_ballModel.reset();
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

	// ボールモデル作成
	std::unique_ptr<EffectFactory> fx = std::make_unique<EffectFactory>(device);
	fx->SetDirectory(L"Resources/Models");
	m_ballModel = Model::CreateFromCMO(device, L"Resources/Models/Ball.cmo", *fx);

	// エフェクトの設定
	m_ballModel->UpdateEffects([](IEffect* effect)
		{
			auto lights = dynamic_cast<IEffectLights*>(effect);
			if (lights)
			{
				lights->SetPerPixelLighting(true);
			}
		}
	);
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
