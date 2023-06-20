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

	// �Փ˔���p�I�u�W�F�N�g�̏����l��ݒ肷��
	m_object[0].position = SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
	m_object[1].position = SimpleMath::Vector3(-1.0f, 0.0f, 0.0f);

	// �Փ˔���p���b�V���I�u�W�F�N�g���쐬����
	m_objCollision = std::make_unique<Imase::ObjCollision>(L"Resources/CollisionMeshes/Floor.obj");

	// ����
	m_line[0] = SimpleMath::Vector3(0, 1, 0);
	m_line[1] = SimpleMath::Vector3(0, -1, 0);
}

void ModelSampleScene::Update(float elapsedTime)
{
	elapsedTime;

	// �f�o�b�O�J�����̍X�V
	m_debugCamera->Update();

	auto kb = Keyboard::Get().GetState();
	auto kbTracker = GetUserResources()->GetKeyboardStateTracker();

	// �X�y�[�X�L�[�������ꂽ��I���I�u�W�F�N�g��ύX����
	if (kbTracker->pressed.Space)
	{
		if (m_selectNo)
		{
			m_selectNo = 0;
		}
		else
		{
			m_selectNo = 1;
		}
	}

	// �I�𒆂̃I�u�W�F�N�g���擾����
	CollisionTest::Object* p = &m_object[m_selectNo];

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
	p->rotate = q * p->rotate;

	// �O�i�E��i
	if (kb.Up) p->position += SimpleMath::Vector3::Transform(SimpleMath::Vector3(0.0f, 0.0f, 0.1f), p->rotate);
	if (kb.Down) p->position += SimpleMath::Vector3::Transform(-SimpleMath::Vector3(0.0f, 0.0f, 0.1f), p->rotate);

	// �����̈ړ�
	m_line[0].x = p->position.x;
	m_line[1].x = p->position.x;
	m_line[0].z = p->position.z;
	m_line[1].z = p->position.z;
}

void ModelSampleScene::Render()
{
	auto debugFont = GetUserResources()->GetDebugFont();
	debugFont->AddString(L"ModelSampleScene", SimpleMath::Vector2(0.0f, debugFont->GetFontHeight()));

	// �I�u�W�F�N�g���m�̏Փ˔�����s��
	SimpleMath::Vector3 hitPosition;
	SimpleMath::Vector3 normal;
	SimpleMath::Matrix tilt;
	if (m_objCollision->IntersectLineSegment(m_line[0], m_line[1], &hitPosition, &normal))
	{
		SimpleMath::Quaternion q = SimpleMath::Quaternion::FromToRotation(SimpleMath::Vector3::UnitY, normal);
		tilt = SimpleMath::Matrix::CreateFromQuaternion(q);
		debugFont->AddString(L"Hit!", SimpleMath::Vector2(0.0f, debugFont->GetFontHeight() * 2));
	}

	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
	auto states = GetUserResources()->GetCommonStates();

	// �r���[�s���ݒ�
	m_view = m_debugCamera->GetCameraMatrix();

	// �O���b�h�̏���`��
	//m_gridFloor->Render(context, m_view, m_proj);

	SimpleMath::Matrix rotate, trans, world;

	// �Ԃ̕`��
	rotate = SimpleMath::Matrix::CreateFromQuaternion(m_object[0].rotate);
	trans = SimpleMath::Matrix::CreateTranslation(hitPosition);
	world = rotate * tilt * trans;
	m_carModel->Draw(context, *states, world, m_view, m_proj);

	// ���̕`��
	// �I�𒆂̃I�u�W�F�N�g���擾����
	CollisionTest::Object* p = &m_object[m_selectNo];

	// �p���̃x�N�g�������߂�
	SimpleMath::Vector3 forward = SimpleMath::Vector3::Transform(SimpleMath::Vector3(0.0f, 0.0f, 1.5f), p->rotate);
	SimpleMath::Vector3 horizontal = SimpleMath::Vector3::Transform(SimpleMath::Vector3(1.5f, 0.0f, 0.0f), p->rotate);
//	SimpleMath::Vector3 vertical = SimpleMath::Vector3::Transform(SimpleMath::Vector3(0.0f, 1.5f, 0.0f), p->rotate);

	m_displayCollision->AddLineSegment(p->position, p->position + forward, Colors::Yellow);
	m_displayCollision->AddLineSegment(p->position, p->position + horizontal, Colors::Red);
	m_displayCollision->AddLineSegment(m_line[0], m_line[1], Colors::Blue);

	// �Փ˔���̓o�^
	m_objCollision->AddDisplayCollision(m_displayCollision.get());

	// �Փ˔���̕\��
	m_displayCollision->DrawCollision(context, states, m_view, m_proj);
}

void ModelSampleScene::Finalize()
{
	m_gridFloor.reset();
	m_carModel.reset();
	m_displayCollision.reset();
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

	// �ԃ��f���̍쐬
	m_carModel = Model::CreateFromCMO(device, L"Resources/Models/Car.cmo", *fx);

	// �Փ˔���̕\���I�u�W�F�N�g�̍쐬
	m_displayCollision = std::make_unique<Imase::DisplayCollision>(device, context);
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

