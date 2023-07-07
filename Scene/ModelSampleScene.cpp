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

	// �f�o�b�O�J�����̍쐬
	RECT rect = GetUserResources()->GetDeviceResources()->GetOutputSize();
	m_debugCamera = std::make_unique<Imase::DebugCamera>(rect.right, rect.bottom);

	// �J�����Ƀv���C���[�Ɉʒu�Ɖ�]��n��
	m_camera.SetPlayer(m_tankPosition, m_tankRotate);

	// �e�{�[���̏����ʒu�s���ݒ�
	m_initialMatrix[PARENT] = SimpleMath::Matrix::Identity;
	m_initialMatrix[CHILD] = SimpleMath::Matrix::CreateTranslation(0.0f, 1.0f, 0.0f);

}

void ModelSampleScene::Update(float elapsedTime)
{
	elapsedTime;

	// �f�o�b�O�J�����̍X�V
	m_debugCamera->Update();

	auto kb = Keyboard::Get().GetState();
	auto kbTracker = GetUserResources()->GetKeyboardStateTracker();

	// ----- ��Ԃ̈ړ� ----- //

	// �O�i
	if (kb.A)
	{
		m_tankRotate *= SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::UnitY, XMConvertToRadians(1.0f));
	}
	// ��i
	if (kb.D)
	{
		m_tankRotate *= SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::UnitY, XMConvertToRadians(-1.0f));
	}
	// �E����
	if (kb.W)
	{
		m_tankPosition += SimpleMath::Vector3::Transform(SimpleMath::Vector3(0.0f, 0.0f, 0.02f), m_tankRotate);
	}
	// ������
	if (kb.S)
	{
		m_tankPosition += SimpleMath::Vector3::Transform(SimpleMath::Vector3(0.0f, 0.0f, -0.02f), m_tankRotate);
	}

	// �J�����̍X�V
	m_camera.Update(elapsedTime);

}

void ModelSampleScene::Render()
{
	auto debugFont = GetUserResources()->GetDebugFont();
	debugFont->AddString(L"ModelSampleScene", SimpleMath::Vector2(0.0f, debugFont->GetFontHeight()));

	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
	auto states = GetUserResources()->GetCommonStates();

	// �r���[�s���ݒ�
	m_view = m_debugCamera->GetCameraMatrix();
	//m_view = SimpleMath::Matrix::CreateLookAt(
	//	m_camera.GetEyePosition(),
	//	m_camera.GetTargetPosition(),
	//	SimpleMath::Vector3::UnitY
	//);

	// �O���b�h�̏���`��
	m_gridFloor->Render(context, m_view, m_proj);

	//// ��Ԃ̕`��
	//SimpleMath::Matrix rotate = SimpleMath::Matrix::CreateFromQuaternion(m_tankRotate);
	//SimpleMath::Matrix trans = SimpleMath::Matrix::CreateTranslation(m_tankPosition);
	//SimpleMath::Matrix world = rotate * trans;
	//m_tankModel->Draw(context, *states, world, m_view, m_proj, false,[&]()
	//	{
	//		context->RSSetState(states->CullNone());
	//	}
	//);

	auto timer = GetUserResources()->GetStepTimer();

	// �{�[���̃{�[���s���X���̉�]�������ĉ�]������
	float angle = static_cast<float>(timer->GetTotalSeconds());
	m_transformMatrix[PARENT] = SimpleMath::Matrix::CreateRotationX(XM_PIDIV2 * sinf(angle));
	m_transformMatrix[CHILD] = SimpleMath::Matrix::CreateRotationX(XM_PIDIV2 * sinf(angle));

	SimpleMath::Matrix world[BONE_CNT];

	// �e���[���h�s����쐬�i�e�{�[���̉�]��ړ��~�{�[���̏����ʒu�j
	world[PARENT] = m_transformMatrix[PARENT] * m_initialMatrix[PARENT];
	world[CHILD] = m_transformMatrix[CHILD] * m_initialMatrix[CHILD];

	// �q�̍s��ɐe�̍s����|����
	world[CHILD] = world[CHILD] * world[PARENT];

	// �{�[����`��
	for (int i = 0; i < BONE_CNT; i++)
	{
		m_boneModel->Draw(context, *states, world[i], m_view, m_proj);
	}

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

	// �G�t�F�N�g�t�@�N�g���[�̍쐬
	std::unique_ptr<EffectFactory> fx = std::make_unique<EffectFactory>(device);
	fx->SetDirectory(L"Resources/Models");

	// ��ԃ��f���̍쐬
	//m_tankModel = Model::CreateFromCMO(device, L"Resources/Models/Tank.cmo", *fx);

	// �{�[�����f���̍쐬
	m_boneModel = Model::CreateFromCMO(device, L"Resources/Models/Bone.cmo", *fx);
}

void ModelSampleScene::CreateWindowSizeDependentResources()
{
	// �ˉe�s����쐬
	RECT rect = GetUserResources()->GetDeviceResources()->GetOutputSize();
	m_proj = SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		XMConvertToRadians(45.0f),
		static_cast<float>(rect.right) / static_cast<float>(rect.bottom),
		0.1f, 200.0f
	);
}

void ModelSampleScene::OnDeviceLost()
{
	Finalize();
}

