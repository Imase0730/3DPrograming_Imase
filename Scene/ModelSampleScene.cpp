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

	// ��s�@�̏����p���̐ݒ�
	m_rotate = SimpleMath::Quaternion::FromToRotation(SimpleMath::Vector3(0.0f, 0.0f, 1.0f), SimpleMath::Vector3(0.0f, 0.0f, -1.0f));

	// �Փ˔���p�I�u�W�F�N�g�̏����l��ݒ肷��
	m_object[0].position = SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
	m_object[1].position = SimpleMath::Vector3(-1.0f, 0.0f, 0.0f);

	// �Փ˔���I�u�W�F�N�g���쐬����
	m_modelCollision[0] = Imase::ModelCollisionFactory::CreateCollision(Imase::ModelCollision::CollisionType::OBB, m_pacmanModel.get());
	m_modelCollision[1] = Imase::ModelCollisionFactory::CreateCollision(Imase::ModelCollision::CollisionType::Sphere, m_planeModel.get());
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

	// �R���W�������̍X�V
	m_modelCollision[0]->UpdateBoundingInfo(m_object[0].position, m_object[0].rotate);
	m_modelCollision[1]->UpdateBoundingInfo(m_object[1].position, m_object[1].rotate);

}

void ModelSampleScene::Render()
{
	auto debugFont = GetUserResources()->GetDebugFont();
	debugFont->AddString(L"ModelSampleScene", SimpleMath::Vector2(0.0f, debugFont->GetFontHeight()));

	// �I�u�W�F�N�g���m�̏Փ˔�����s��
	if (m_modelCollision[0]->Intersects(m_modelCollision[1].get()))
	{
		debugFont->AddString(L"Hit!", SimpleMath::Vector2(0.0f, debugFont->GetFontHeight() * 2));
	}

	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
	auto states = GetUserResources()->GetCommonStates();

	// �r���[�s���ݒ�
	m_view = m_debugCamera->GetCameraMatrix();

	// �O���b�h�̏���`��
	m_gridFloor->Render(context, m_view, m_proj);

	// �p�b�N�}���̕`��
	SimpleMath::Matrix rotate = SimpleMath::Matrix::CreateFromQuaternion(m_object[0].rotate);
	SimpleMath::Matrix trans = SimpleMath::Matrix::CreateTranslation(m_object[0].position);
	SimpleMath::Matrix world = rotate * trans;
	m_pacmanModel->Draw(context, *states, world, m_view, m_proj);

	// ��s�@�̕`��
	rotate = SimpleMath::Matrix::CreateFromQuaternion(m_object[1].rotate);
	trans = SimpleMath::Matrix::CreateTranslation(m_object[1].position);
	world = rotate * trans;
	m_planeModel->Draw(context, *states, world, m_view, m_proj);

	// ���̕`��
	// �I�𒆂̃I�u�W�F�N�g���擾����
	CollisionTest::Object* p = &m_object[m_selectNo];

	// �p���̃x�N�g�������߂�
	SimpleMath::Vector3 forward = SimpleMath::Vector3::Transform(SimpleMath::Vector3(0.0f, 0.0f, 1.5f), p->rotate);
	SimpleMath::Vector3 horizontal = SimpleMath::Vector3::Transform(SimpleMath::Vector3(1.5f, 0.0f, 0.0f), p->rotate);
	SimpleMath::Vector3 vertical = SimpleMath::Vector3::Transform(SimpleMath::Vector3(0.0f, 1.5f, 0.0f), p->rotate);

	context->OMSetBlendState(states->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(states->DepthDefault(), 0);
	context->RSSetState(states->CullNone());

	m_basicEffect->SetView(m_view);
	m_basicEffect->SetProjection(m_proj);
	m_basicEffect->Apply(context);

	context->IASetInputLayout(m_inputLayout.Get());

	m_primitiveBatch->Begin();

	DX::DrawRay(m_primitiveBatch.get(), p->position, forward, false, Colors::Yellow);
	DX::DrawRay(m_primitiveBatch.get(), p->position, horizontal, false, Colors::Red);
	DX::DrawRay(m_primitiveBatch.get(), p->position, vertical, false, Colors::Blue);

	m_primitiveBatch->End();

	// �Փ˔���̓o�^
	m_modelCollision[0]->AddDisplayCollision(m_displayCollision.get());
	m_modelCollision[1]->AddDisplayCollision(m_displayCollision.get());

	// �Փ˔���̕\��
	m_displayCollision->DrawCollision(context, states, m_view, m_proj);
}

void ModelSampleScene::Finalize()
{
	m_basicEffect.reset();
	m_primitiveBatch.reset();
	m_inputLayout.Reset();
	m_gridFloor.reset();
	m_planeModel.reset();
	m_displayCollision.reset();
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

	// �p�b�N�}�����f���̍쐬
	m_pacmanModel = Model::CreateFromCMO(device, L"Resources/Models/Pacman.cmo", *fx);

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

// ���Ƌ��̏Փ˔���֐�
bool ModelSampleScene::HitCheck_Sphere2Sphere(
	const DirectX::BoundingSphere& sphere1,
	const DirectX::BoundingSphere& sphere2
)
{
	// 2�̋��̒��S�̊Ԃ̋����̕������v�Z
	SimpleMath::Vector3 d = SimpleMath::Vector3(sphere1.Center) - SimpleMath::Vector3(sphere2.Center);
	float distSq = d.Dot(d);
	// �������������������������a�̍��v�����������ꍇ�ɋ��͌������Ă���
	float radiusSum = sphere1.Radius + sphere2.Radius;
	return distSq <= radiusSum * radiusSum;
}
