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
}

void ModelSampleScene::Update(float elapsedTime)
{
	elapsedTime;
}

void ModelSampleScene::Render()
{
	auto debugFont = GetUserResources()->GetDebugFont();
	debugFont->AddString(L"ModelSampleScene", SimpleMath::Vector2(0.0f, debugFont->GetFontHeight()));

	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();

	// �r���[�s����쐬
	m_view = SimpleMath::Matrix::CreateLookAt(
		SimpleMath::Vector3(0.0f, 10.0f, 10.0f),
		SimpleMath::Vector3(0.0f, 0.0f, 0.0f),
		SimpleMath::Vector3::UnitY
	);

	// �O���b�h�̏���`��
	m_gridFloor->Render(context, m_view, m_proj);
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

	// �O���b�h�̏����쐬
	m_gridFloor = std::make_unique<Imase::GridFloor>(device, context, states);
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
