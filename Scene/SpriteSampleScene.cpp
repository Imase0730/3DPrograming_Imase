#include "pch.h"
#include "SpriteSampleScene.h"

using namespace DirectX;

// ばね係数
const float SpriteSampleScene::K = 0.08f;

// 摩擦係数
const float SpriteSampleScene::FRICTION = 0.97f;

SpriteSampleScene::SpriteSampleScene()
	: m_robotScale(1.0f)
	, m_robotVelocity(0.0f)
	, m_number(nullptr)
{
}

void SpriteSampleScene::Initialize()
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();

	// 数字を表示するタスクを登録する
	m_number = m_taskManager.AddTask<Number>(&m_spriteBatch, m_numberSRV.GetAddressOf());
	m_number->SetNumber(12345678);
	m_number->SetPosition(SimpleMath::Vector2(400.0f, 300.0f));
}

void SpriteSampleScene::Update(float elapsedTime)
{
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

	// タスクの更新
	m_taskManager.Update(elapsedTime);
}

void SpriteSampleScene::Render()
{
	auto* debugFont = GetUserResources()->GetDebugFont();
	debugFont->AddString(L"SpriteSampleScene", SimpleMath::Vector2(0.0f, debugFont->GetFontHeight()));

	m_spriteBatch->Begin();

	// カードの絵を描画する
	m_spriteBatch->Draw(
		m_cardSRV[0].Get(),
		SimpleMath::Vector2(0.0f, 0.0f),
		nullptr,
		Colors::White,
		0.0f,
		SimpleMath::Vector2(0.0f, 0.0f),
		1.0f,
		SpriteEffects_None, 0.0f
	);

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

	// タスクの描画
	m_taskManager.Render();

	m_spriteBatch->End();
}

void SpriteSampleScene::Finalize()
{
	m_numberSRV.Reset();
	m_cardSRV[0].Reset();
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

	// 数字の絵の読み込み
	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"Resources/Textures/number.dds", nullptr, m_numberSRV.ReleaseAndGetAddressOf())
	);

	// カードの絵の読み込み
	//DX::ThrowIfFailed(
	//	CreateDDSTextureFromFile(device, L"Resources/Textures/card_heart_01.dds", nullptr, m_cardSRV[0].ReleaseAndGetAddressOf())
	//);
	//DX::ThrowIfFailed(
	//	CreateDDSTextureFromFile(device, L"Resources/Textures/card_heart_02.dds", nullptr, m_cardSRV[1].ReleaseAndGetAddressOf())
	//);
	//DX::ThrowIfFailed(
	//	CreateDDSTextureFromFile(device, L"Resources/Textures/card_heart_03.dds", nullptr, m_cardSRV[2].ReleaseAndGetAddressOf())
	//);

	// カードの絵の読み込み
	const wchar_t* TEXTURE_NAME = L"Resources/Textures/card_heart_01.dds";
	m_resourceManager.LoadData(TEXTURE_NAME);
	DX::ThrowIfFailed(
		CreateDDSTextureFromMemory(
			device,
			m_resourceManager.GetData(TEXTURE_NAME).data(),
			m_resourceManager.GetData(TEXTURE_NAME).size(),
			nullptr,
			m_cardSRV[0].ReleaseAndGetAddressOf()	// <--- リソースを管理するならSRVを管理した方が良い
		)
	);
	m_resourceManager.ReleaseData(TEXTURE_NAME);	// < --- 読み込んだテクスチャデータは破棄してもOK
}

void SpriteSampleScene::CreateWindowSizeDependentResources()
{
}

void SpriteSampleScene::OnDeviceLost()
{
	Finalize();
}
