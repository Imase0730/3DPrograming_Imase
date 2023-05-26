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

	// �f�o�b�O�J�����̍쐬
	RECT rect = GetUserResources()->GetDeviceResources()->GetOutputSize();
	m_debugCamera = std::make_unique<Imase::DebugCamera>(rect.right, rect.bottom);

	// �{�[���̐i�s����������������
	m_forward = SimpleMath::Vector3(0.0f, 0.0f, -1.0f);
}

void ModelSampleScene::Update(float elapsedTime)
{
	elapsedTime;

	// �f�o�b�O�J�����̍X�V
	m_debugCamera->Update();

	auto kb = Keyboard::Get().GetState();

	// ���L�[�̍��E�L�[�Ői�s��������]������
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

	// ���L�[�㉺�ŉ���������
	if (kb.Up) m_speed += 0.001f;
	if (kb.Down) m_speed -= 0.001f;

	// �{�[���̉�]�̎��x�N�g�������߂�
	m_horizontal = SimpleMath::Vector3(m_forward.z, 0.0f, -m_forward.x);

	// ���C�W�����|���Č���������
	m_speed *= 0.98f;

	// ���x�x�N�g��
	SimpleMath::Vector3 v = m_forward * m_speed;

	// �{�[���̈ړ�
	m_pos += v;

	// �{�[������]������i�{�[���̔��a��0.5�Ȃ̂Ń{�[���̈ړ��������Q�{���ċ��߂�j
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

	// �r���[�s���ݒ�
	m_view = m_debugCamera->GetCameraMatrix();

	// �O���b�h�̏���`��
	m_gridFloor->Render(context, m_view, m_proj);

	// �{�[���̕`��
	SimpleMath::Matrix rotate = SimpleMath::Matrix::CreateFromQuaternion(m_rotate);
	SimpleMath::Matrix transA = SimpleMath::Matrix::CreateTranslation(SimpleMath::Vector3(0.0f, 0.5f, 0.0f));
	SimpleMath::Matrix transB = SimpleMath::Matrix::CreateTranslation(m_pos);
	SimpleMath::Matrix world = rotate * transA * transB;
	m_ballModel->Draw(context, *states, world, m_view, m_proj);

	// ���̕`��
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

	// �{�[�����f���쐬
	std::unique_ptr<EffectFactory> fx = std::make_unique<EffectFactory>(device);
	fx->SetDirectory(L"Resources/Models");
	m_ballModel = Model::CreateFromCMO(device, L"Resources/Models/Ball.cmo", *fx);

	// �G�t�F�N�g�̐ݒ�
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
