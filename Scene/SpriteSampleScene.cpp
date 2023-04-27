#include "../pch.h"
#include "SpriteSampleScene.h"

using namespace DirectX;

// ばね係数
const float SpriteSampleScene::K = 0.08f;

// 摩擦係数
const float SpriteSampleScene::FRICTION = 0.97f;

SpriteSampleScene::SpriteSampleScene()
	: m_robotScale(1.0f)
	, m_robotVelocity(0.0f)
{
}

void SpriteSampleScene::Initialize()
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

void SpriteSampleScene::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	auto mouse = Mouse::Get().GetState();

	// マウスの左ボタンが押されたら
	if (mouse.leftButton)
	{
		// ロボットを大きくする
		m_robotScale += 0.01f;
	}
	else
	{
		// ばねの動きでスケールを変化させる
		float a = -K * (m_robotScale - 1.0f);
		m_robotVelocity += a;
		m_robotVelocity *= FRICTION;
		m_robotScale += m_robotVelocity;
	}

}

void SpriteSampleScene::Render()
{
	auto* debugFont = GetUserResources()->GetDebugFont();
	debugFont->AddString(L"SpriteSampleScene", SimpleMath::Vector2(0.0f, debugFont->GetFontHeight()));

	m_spriteBatch->Begin();

	// 猫の絵を描画する
	m_spriteBatch->Draw(m_catSRV.Get(), SimpleMath::Vector2(0.0f, 0.0f));

	// ロボットの絵を描画する
	RECT srcRect = { 128, 128, 256, 256};
	m_spriteBatch->Draw(
		m_robotSRV.Get(),
		SimpleMath::Vector2(400.0f, 300.0f),
		&srcRect,
		Colors::White,
		0.0f,
		SimpleMath::Vector2(64.0f, 64.0f),
		m_robotScale
	);

	m_spriteBatch->End();
}

void SpriteSampleScene::Finalize()
{
	m_robotSRV.Reset();
	m_catSRV.Reset();
	m_spriteBatch.reset();
}

void SpriteSampleScene::CreateDeviceDependentResources()
{
	auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();
	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();

	// スプライトバッチの作成
	m_spriteBatch = std::make_unique<SpriteBatch>(context);

	// 猫の絵の読み込み
	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"Resources/Textures/cat.dds", nullptr, m_catSRV.ReleaseAndGetAddressOf())
	);

	// ロボットの絵の読み込み
	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"Resources/Textures/robot.dds", nullptr, m_robotSRV.ReleaseAndGetAddressOf())
	);
}

void SpriteSampleScene::CreateWindowSizeDependentResources()
{
}

void SpriteSampleScene::OnDeviceLost()
{
	Finalize();
}
