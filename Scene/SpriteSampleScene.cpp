#include "../pch.h"
#include "SpriteSampleScene.h"

using namespace DirectX;

// �΂ˌW��
const float SpriteSampleScene::K = 0.08f;

// ���C�W��
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

	// �}�E�X�̍��{�^���������ꂽ��
	if (mouse.leftButton)
	{
		// ���{�b�g��傫������
		m_robotScale += 0.01f;
	}
	else
	{
		// �΂˂̓����ŃX�P�[����ω�������
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

	// �L�̊G��`�悷��
	m_spriteBatch->Draw(m_catSRV.Get(), SimpleMath::Vector2(0.0f, 0.0f));

	// ���{�b�g�̊G��`�悷��
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

	// �X�v���C�g�o�b�`�̍쐬
	m_spriteBatch = std::make_unique<SpriteBatch>(context);

	// �L�̊G�̓ǂݍ���
	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"Resources/Textures/cat.dds", nullptr, m_catSRV.ReleaseAndGetAddressOf())
	);

	// ���{�b�g�̊G�̓ǂݍ���
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
