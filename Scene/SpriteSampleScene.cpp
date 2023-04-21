#include "../pch.h"
#include "SpriteSampleScene.h"

using namespace DirectX;

void SpriteSampleScene::Initialize()
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

void SpriteSampleScene::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);
}

void SpriteSampleScene::Render()
{
	auto* debugFont = GetUserResources()->GetDebugFont();
	debugFont->AddString(L"SpriteSampleScene", SimpleMath::Vector2(0.0f, debugFont->GetFontHeight()));

	m_spriteBatch->Begin();

	// �L�̊G��`�悷��
	m_spriteBatch->Draw(m_catSRV.Get(), SimpleMath::Vector2(0.0f, 0.0f));

	m_spriteBatch->End();
}

void SpriteSampleScene::Finalize()
{
	m_catSRV.Get();
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
}

void SpriteSampleScene::CreateWindowSizeDependentResources()
{
}

void SpriteSampleScene::OnDeviceLost()
{
	Finalize();
}
