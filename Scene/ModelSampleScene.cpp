#include "pch.h"
#include "ModelSampleScene.h"

using namespace DirectX;

ModelSampleScene::ModelSampleScene()
{
}

void ModelSampleScene::Initialize()
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();

	// �f�o�b�O�J�����̍쐬
	RECT rect = GetUserResources()->GetDeviceResources()->GetOutputSize();
	m_debugCamera = std::make_unique<Imase::DebugCamera>(rect.right, rect.bottom);
}

void ModelSampleScene::Update(float elapsedTime)
{
	elapsedTime;

	// �f�o�b�O�J�����̍X�V
	m_debugCamera->Update();
}

void ModelSampleScene::Render()
{
	auto debugFont = GetUserResources()->GetDebugFont();
	debugFont->AddString(L"ModelSampleScene", SimpleMath::Vector2(0.0f, debugFont->GetFontHeight()));

	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
	auto states = GetUserResources()->GetCommonStates();

	// �r���[�s���ݒ�
	m_view = m_debugCamera->GetCameraMatrix();

	// �O���b�h�̏���`��
	m_gridFloor->Render(context, m_view, m_proj);

	// �T�C�R���̕`��
	SimpleMath::Matrix world;
	m_diceModel->Draw(context, *states, world, m_view, m_proj);
}

void ModelSampleScene::Finalize()
{
	m_gridFloor.reset();
	m_diceModel.reset();
}

void ModelSampleScene::CreateDeviceDependentResources()
{
	auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();
	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
	auto states = GetUserResources()->GetCommonStates();

	// �O���b�h�̏����쐬
	m_gridFloor = std::make_unique<Imase::GridFloor>(device, context, states);

	// �T�C�R�����f���쐬
	std::unique_ptr<EffectFactory> fx = std::make_unique<EffectFactory>(device);
	fx->SetDirectory(L"Resources/Models");
	m_diceModel = Model::CreateFromCMO(device, L"Resources/Models/Dice.cmo", *fx);
}

void ModelSampleScene::CreateWindowSizeDependentResources()
{
	// �ˉe�s����쐬
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
