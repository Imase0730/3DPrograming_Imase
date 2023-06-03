#include "pch.h"
#include "ModelSampleScene.h"
#include "DebugDraw.h"

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

	// ��s�@�̏����p���̐ݒ�
	m_rotate = SimpleMath::Quaternion::FromToRotation(SimpleMath::Vector3(0.0f, 0.0f, 1.0f), SimpleMath::Vector3(0.0f, 0.0f, -1.0f));
}

void ModelSampleScene::Update(float elapsedTime)
{
	elapsedTime;

	// �f�o�b�O�J�����̍X�V
	m_debugCamera->Update();

	auto kb = Keyboard::Get().GetState();

	SimpleMath::Quaternion q;

	// �s�b�`
	if (kb.W) q *= SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::UnitX, XMConvertToRadians(1.0f));
	if (kb.S) q *= SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::UnitX, XMConvertToRadians(-1.0f));

	// ���[
	if (kb.Left) q *= SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::UnitY, XMConvertToRadians(1.0f));
	if (kb.Right) q *= SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::UnitY, XMConvertToRadians(-1.0f));

	// ���[��
	if (kb.D) q *= SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::UnitZ, XMConvertToRadians(1.0f));
	if (kb.A) q *= SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::UnitZ, XMConvertToRadians(-1.0f));

	// �p���ɉ�]��������
	m_rotate = q * m_rotate;

	// �O�i�E��i
	if (kb.Up) m_planePos += SimpleMath::Vector3::Transform(SimpleMath::Vector3(0.0f, 0.0f, 0.1f), m_rotate);
	if (kb.Down) m_planePos += SimpleMath::Vector3::Transform(-SimpleMath::Vector3(0.0f, 0.0f, 0.1f), m_rotate);
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

	// ��s�@�̕`��
	SimpleMath::Matrix rotate = SimpleMath::Matrix::CreateFromQuaternion(m_rotate);
	SimpleMath::Matrix trans = SimpleMath::Matrix::CreateTranslation(m_planePos);
	SimpleMath::Matrix world = rotate * trans;
	m_planeModel->Draw(context, *states, world, m_view, m_proj);

	// ���̕`��

	// �p���̃x�N�g�������߂�
	SimpleMath::Vector3 forward = SimpleMath::Vector3::Transform(SimpleMath::Vector3(0.0f, 0.0f, 1.5f), m_rotate);
	SimpleMath::Vector3 horizontal = SimpleMath::Vector3::Transform(SimpleMath::Vector3(1.5f, 0.0f, 0.0f), m_rotate);
	SimpleMath::Vector3 vertical = SimpleMath::Vector3::Transform(SimpleMath::Vector3(0.0f, 1.5f, 0.0f), m_rotate);

	context->OMSetBlendState(states->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(states->DepthDefault(), 0);
	context->RSSetState(states->CullNone());

	m_basicEffect->SetView(m_view);
	m_basicEffect->SetProjection(m_proj);
	m_basicEffect->Apply(context);

	context->IASetInputLayout(m_inputLayout.Get());

	m_primitiveBatch->Begin();

	DX::DrawRay(m_primitiveBatch.get(), m_planePos, forward, false, Colors::Yellow);
	DX::DrawRay(m_primitiveBatch.get(), m_planePos, horizontal, false, Colors::Red);
	DX::DrawRay(m_primitiveBatch.get(), m_planePos, vertical, false, Colors::Blue);

	m_primitiveBatch->End();
}

void ModelSampleScene::Finalize()
{
	m_basicEffect.reset();
	m_primitiveBatch.reset();
	m_inputLayout.Reset();
	m_gridFloor.reset();
	m_planeModel.reset();
}

void ModelSampleScene::CreateDeviceDependentResources()
{
	auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();
	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
	auto states = GetUserResources()->GetCommonStates();

	// �x�[�V�b�N�G�t�F�N�g�̍쐬
	m_basicEffect = std::make_unique<BasicEffect>(device);
	m_basicEffect->SetVertexColorEnabled(true);

	// �v���~�e�B�u�o�b�`�̍쐬
	m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);

	// ���̓��C�A�E�g�̍쐬
	DX::ThrowIfFailed(
		CreateInputLayoutFromEffect<VertexPositionColor>(
			device,
			m_basicEffect.get(),
			m_inputLayout.ReleaseAndGetAddressOf())
	);

	// �O���b�h�̏����쐬
	m_gridFloor = std::make_unique<Imase::GridFloor>(device, context, states);

	// ��s�@���f���쐬
	std::unique_ptr<EffectFactory> fx = std::make_unique<EffectFactory>(device);
	fx->SetDirectory(L"Resources/Models");
	m_planeModel = Model::CreateFromCMO(device, L"Resources/Models/Plane.cmo", *fx);
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
