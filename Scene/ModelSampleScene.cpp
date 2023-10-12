#include "pch.h"
#include "ModelSampleScene.h"
#include "DebugDraw.h"
#include "ReadData.h"

using namespace DirectX;

ModelSampleScene::ModelSampleScene()
	: m_fireFlag(false), m_distance(0.0f)
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
	m_camera.SetPlayer(m_robotPosition, m_robotRotate);

	// ���{�b�g�̊e�p�[�c�̍쐬
	m_parts[ROOT] = std::make_unique<Imase::ModelPart>();
	m_parts[HEAD] = std::make_unique<Imase::ModelPart>(m_headModel.get());
	m_parts[BODY] = std::make_unique<Imase::ModelPart>(m_bodyModel.get());
	m_parts[LEG] = std::make_unique<Imase::ModelPart>(m_legModel.get());
	m_parts[ARM_R] = std::make_unique<Imase::ModelPart>(m_armRModel.get());
	m_parts[ARM_L] = std::make_unique<Imase::ModelPart>(m_armLModel.get());
	m_parts[MISSILE] = std::make_unique<Imase::ModelPart>(m_missileModel.get());

	// ���{�b�g�̊e�p�[�c��A������
	m_parts[ROOT]->SetChild(m_parts[LEG].get());
	m_parts[LEG]->SetChild(m_parts[BODY].get());
	m_parts[BODY]->SetChild(m_parts[HEAD].get());
		m_parts[HEAD]->SetSibling(m_parts[ARM_R].get());
	m_parts[ARM_R]->SetSibling(m_parts[ARM_L].get());
	m_parts[ARM_L]->SetChild(m_parts[MISSILE].get());

	// �e�p�[�c�̏����ʒu�s����쐬����
	m_parts[BODY]->SetInitialMatrix(
		SimpleMath::Matrix::CreateTranslation(0.0f, 0.32f, 0.0f));
	m_parts[HEAD]->SetInitialMatrix(
		SimpleMath::Matrix::CreateTranslation(0.0f, 0.75f, 0.0f));
	m_parts[ARM_R]->SetInitialMatrix(
		SimpleMath::Matrix::CreateTranslation(-0.18f, 0.72f, 0.0f));
	m_parts[ARM_L]->SetInitialMatrix(
		SimpleMath::Matrix::CreateTranslation(0.18f, 0.72f, 0.0f));
	m_parts[MISSILE]->SetInitialMatrix(
		SimpleMath::Matrix::CreateTranslation(0.28f, 0.52f, 0.39f));

	// �A������Ă���p�[�c�̏����ʒu�s���e�̍��W�n�ɑS�ĕϊ�����
	m_parts[ROOT]->SetupMatrix();
}

void ModelSampleScene::Update(float elapsedTime)
{
	elapsedTime;

	// �f�o�b�O�J�����̍X�V
	m_debugCamera->Update();

	auto kb = Keyboard::Get().GetState();
	auto kbTracker = GetUserResources()->GetKeyboardStateTracker();

	// ----- �J�����̐؂�ւ� ----- //

	if (kbTracker->pressed.Z)
	{
		m_camera.SetType(GameCamera::Type::Type_A);
	}

	if (kbTracker->pressed.X)
	{
		m_camera.SetType(GameCamera::Type::Type_B);
	}

	// ----- ���{�b�g�̈ړ� ----- //

	// �O�i
	if (kb.A)
	{
		m_robotRotate *= SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::UnitY, XMConvertToRadians(1.0f));
	}
	// ��i
	if (kb.D)
	{
		m_robotRotate *= SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::UnitY, XMConvertToRadians(-1.0f));
	}
	// �E����
	if (kb.W)
	{
		m_robotPosition += SimpleMath::Vector3::Transform(SimpleMath::Vector3(0.0f, 0.0f, 0.02f), m_robotRotate);
	}
	// ������
	if (kb.S)
	{
		m_robotPosition += SimpleMath::Vector3::Transform(SimpleMath::Vector3(0.0f, 0.0f, -0.02f), m_robotRotate);
	}

	// ----- ���{�b�g�̏㔼�g�̉�] ----- //
	if (kb.Left)
	{
		m_bodyRotate *= SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::UnitY, XMConvertToRadians(1.0f));
	}
	if (kb.Right)
	{
		m_bodyRotate *= SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::UnitY, XMConvertToRadians(-1.0f));
	}

	// ----- ���{�b�g�̍��r�̉�] ----- //
	if (kb.Up)
	{
		m_armRotate *= SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::UnitX, XMConvertToRadians(1.0f));
	}
	if (kb.Down)
	{
		m_armRotate *= SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::UnitX, XMConvertToRadians(-1.0f));
	}

	// �X�y�[�X�L�[�Ń~�T�C���𔭎�
	if (!m_fireFlag && kbTracker->pressed.Space)
	{
		m_fireFlag = true;
		// �~�T�C�������r����O��
		m_parts[ARM_L]->SetChild(nullptr);
		// �~�T�C���̃��[���h�s����擾
		SimpleMath::Matrix m = m_parts[MISSILE]->GetWorldMatrix();
		// �~�T�C���̈ʒu���擾
		m_missilePosition.x = m._41;
		m_missilePosition.y = m._42;
		m_missilePosition.z = m._43;
		// �~�T�C���̉�]���擾
		m_missileRotate = SimpleMath::Quaternion::CreateFromRotationMatrix(m);
	}

	// �~�T�C�����˒��Ȃ�
	if (m_fireFlag)
	{
		// �~�T�C������]������
		m_missileRotate = SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::UnitZ, XMConvertToRadians(5.0f))
						* m_missileRotate;
		// �~�T�C���������Ă�������ֈړ�������
		SimpleMath::Vector3 v(0.0f, 0.0f, 0.02f);
		m_missilePosition += SimpleMath::Vector3::Transform(v, m_missileRotate);
		m_distance += v.Length();
		// �ړ��������R���𒴂�����r�Ƀ~�T�C����߂�
		if (m_distance > 3.0f)
		{
			m_fireFlag = false;
			m_distance = 0.0f;
			m_parts[ARM_L]->SetChild(m_parts[MISSILE].get());
		}
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
	//m_view = m_debugCamera->GetCameraMatrix();
	m_view = SimpleMath::Matrix::CreateLookAt(
		m_camera.GetEyePosition(),
		m_camera.GetTargetPosition(),
		SimpleMath::Vector3::UnitY
	);

	// �O���b�h�̏���`��
	//m_gridFloor->Render(context, m_view, m_proj);

	// ���̕`��
	m_floorModel->Draw(context, *states, SimpleMath::Matrix::Identity, m_view, m_proj,
		false, [&]()
		{
			// �[�x�X�e���V���X�e�[�g�̐ݒ�
			context->OMSetDepthStencilState(m_depthStencilState_Floor.Get(), 0);
			// �e�N�X�`���T���v���̐ݒ�
			ID3D11SamplerState* sampler[] = { states->PointWrap() };
			context->PSSetSamplers(0, 1, sampler);
		}
	);

	// ���{�b�g�̏㔼�g�̉�]
	m_parts[BODY]->SetTransformMatrix(SimpleMath::Matrix::CreateFromQuaternion(m_bodyRotate));

	// ���{�b�g�̍��r�̉�]
	m_parts[ARM_L]->SetTransformMatrix(SimpleMath::Matrix::CreateFromQuaternion(m_armRotate));

	// ���{�b�g���ړ�������
	SimpleMath::Matrix m = SimpleMath::Matrix::CreateFromQuaternion(m_robotRotate)
		                 * SimpleMath::Matrix::CreateTranslation(m_robotPosition);
	m_parts[ROOT]->SetTransformMatrix(m);

	// �e�̕`��֐�
	//DrawShadow(context, states, m_robotPosition, 1.0f);

	// �e�ɂ���s����쐬
	SimpleMath::Matrix shadowMatrix = SimpleMath::Matrix::CreateShadow(SimpleMath::Vector3(0.0f, 1.0f, 0.0f), SimpleMath::Plane(0.0f, 1.0f, 0.0f, -0.01f));
	m_parts[ROOT]->SetTransformMatrix(shadowMatrix * m);

	// ���{�b�g�̉e�̕`��
	m_parts[ROOT]->UpdateMatrix();
	m_parts[ROOT]->Draw(context, *states, m_view, m_proj, [&]()
		{
			// �[�x�X�e���V���̐ݒ�
			context->OMSetDepthStencilState(m_depthStencilState_Shadow.Get(), 1);
			// �u�����h�X�e�[�g�̐ݒ�
			context->OMSetBlendState(states->NonPremultiplied(), nullptr, 0xffffffff);
			// �J�����O���Ȃ�
			context->RSSetState(states->CullNone());
			// �s�N�Z���V�F�[�_�[�̐ݒ�
			context->PSSetShader(m_PS.Get(), nullptr, 0);
		}
	);

	// ���{�b�g�̕`��
	m_parts[ROOT]->SetTransformMatrix(m);
	m_parts[ROOT]->UpdateMatrix();
	m_parts[ROOT]->Draw(context, *states, m_view, m_proj, [&]()
		{
			// �J�����O���Ȃ�
			context->RSSetState(states->CullNone());
			// �s�N�Z���V�F�[�_�[�̐ݒ�
			context->PSSetShader(m_PS_Robot.Get(), nullptr, 0);
		}
	);

	// �~�T�C�����˒��Ȃ�
	if (m_fireFlag)
	{
		// �~�T�C����`�悷��
		SimpleMath::Matrix world = SimpleMath::Matrix::CreateFromQuaternion(m_missileRotate)
								 * SimpleMath::Matrix::CreateTranslation(m_missilePosition);
		m_parts[MISSILE]->GetModel()->Draw(context, *states, world, m_view, m_proj);
	}

}

void ModelSampleScene::Finalize()
{
	m_gridFloor.reset();

	m_headModel.reset();
	m_bodyModel.reset();
	m_legModel.reset();
	m_armRModel.reset();
	m_armLModel.reset();
	m_missileModel.reset();

	// �e�̏I������
	ResetShadow();
}

void ModelSampleScene::CreateDeviceDependentResources()
{
	auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();
	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
	auto states = GetUserResources()->GetCommonStates();

	// �O���b�h�̏����쐬
	m_gridFloor = std::make_unique<Imase::GridFloor>(device, context, states);

	// �G�t�F�N�g�t�@�N�g���[�̍쐬
	EffectFactory fx(device);
	fx.SetDirectory(L"Resources/Models");

	// ���{�b�g�̊e�p�[�c���f���̍쐬
	m_headModel = Model::CreateFromCMO(device, L"Resources/Models/Head.cmo", fx);
	m_bodyModel = Model::CreateFromCMO(device, L"Resources/Models/Body.cmo", fx);
	m_legModel = Model::CreateFromCMO(device, L"Resources/Models/Leg.cmo", fx);
	m_armRModel = Model::CreateFromCMO(device, L"Resources/Models/Arm_R.cmo", fx);
	m_armLModel = Model::CreateFromCMO(device, L"Resources/Models/Arm_L.cmo", fx);
	m_missileModel = Model::CreateFromCMO(device, L"Resources/Models/Missile.cmo", fx);

	// ���̃��f���̍쐬
	m_floorModel = Model::CreateFromCMO(device, L"Resources/Models/Floor.cmo", fx);

	// �e�̏������֐�
	InitializeShadow(device, context);

	// ----- �[�x�X�e���V���̍쐬 ----- //

	D3D11_DEPTH_STENCIL_DESC desc = {};

	// ���i�`�掞�O���P�ɂ���j
	desc.DepthEnable = TRUE;									// �[�x�e�X�g���s��
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;			// �[�x�o�b�t�@���X�V����
	desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;				// �[�x�l�ȉ��Ȃ�X�V����

	desc.StencilEnable = TRUE;									// �X�e���V���e�X�g���s��
	desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;		// 0xff
	desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;	// 0xff

	// �\��
	desc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;		// �O�Ȃ�
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR_SAT;	// OK�@�P�{
	desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;		// NG�@�������Ȃ�
	desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;	// NG�@�������Ȃ�

	desc.BackFace = desc.FrontFace;	// ���ʂ�����

	device->CreateDepthStencilState(&desc, m_depthStencilState_Floor.ReleaseAndGetAddressOf());

	// ���i�P�̂ݕ`�悷��j
	desc.DepthEnable = TRUE;									// �[�x�e�X�g���s��Ȃ�
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;			// �[�x�o�b�t�@���X�V���Ȃ�
//	desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;				// �[�x�l�ȉ��Ȃ�X�V����

//	desc.StencilEnable = TRUE;									// �X�e���V���e�X�g���s��
//	desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;		// 0xff
//	desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;	// 0xff

	// �\��
//	desc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;		// �P�Ȃ�
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR_SAT;	// OK�@�P�{
//	desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;		// NG�@�������Ȃ�
//	desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;	// NG�@�������Ȃ�

	desc.BackFace = desc.FrontFace;	// ���ʂ�����

	device->CreateDepthStencilState(&desc, m_depthStencilState_Shadow.ReleaseAndGetAddressOf());

	// �s�N�Z���V�F�[�_�[�̍쐬
	std::vector<uint8_t> ps = DX::ReadData(L"Resources/Shaders/PixelShader.cso");
	DX::ThrowIfFailed(
		device->CreatePixelShader(ps.data(), ps.size(), nullptr, m_PS.ReleaseAndGetAddressOf())
	);

	// �s�N�Z���V�F�[�_�[�̍쐬�i���{�b�g�p�j
	std::vector<uint8_t> ps_robot = DX::ReadData(L"Resources/Shaders/PS_Robot.cso");
	DX::ThrowIfFailed(
		device->CreatePixelShader(ps_robot.data(), ps_robot.size(), nullptr, m_PS_Robot.ReleaseAndGetAddressOf())
	);
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

// �e�̏������֐�
void ModelSampleScene::InitializeShadow(ID3D11Device* device, ID3D11DeviceContext* context)
{
	// �x�[�V�b�N�G�t�F�N�g�̍쐬
	m_basicEffect = std::make_unique<BasicEffect>(device);
	// ���C�e�B���O(OFF)
	m_basicEffect->SetLightingEnabled(false);
	// ���_�J���[(OFF)
	m_basicEffect->SetVertexColorEnabled(false);
	// �e�N�X�`��(ON)
	m_basicEffect->SetTextureEnabled(true);

	// ���̓��C�A�E�g�̍쐬
	DX::ThrowIfFailed(
		CreateInputLayoutFromEffect<VertexPositionTexture>(
			device,
			m_basicEffect.get(),
			m_inputLayout.ReleaseAndGetAddressOf()
		)
	);

	// �v���~�e�B�u�o�b�`�̍쐬
	m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionTexture>>(context);

	// �e�̃e�N�X�`���̓ǂݍ���
	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(
			device,
			L"Resources/Textures/Shadow.dds",
			nullptr,
			m_shadowTexture.ReleaseAndGetAddressOf()
		)
	);
}

// �e�̏I������
void ModelSampleScene::ResetShadow()
{
	m_basicEffect.reset();
	m_primitiveBatch.reset();
}

// �e�̕`��֐�
void ModelSampleScene::DrawShadow(
	ID3D11DeviceContext* context,
	DirectX::CommonStates* states,
	DirectX::SimpleMath::Vector3 position,
	float radius
)
{
	// �G�t�F�N�g�̐ݒ聕�K��
	m_basicEffect->SetWorld(SimpleMath::Matrix::Identity);
	m_basicEffect->SetView(m_view);
	m_basicEffect->SetProjection(m_proj);
	m_basicEffect->SetTexture(m_shadowTexture.Get());
	m_basicEffect->Apply(context);

	// ���̓��C�A�E�g�̐ݒ�
	context->IASetInputLayout(m_inputLayout.Get());

	// �e�N�X�`���T���v���̐ݒ�
	ID3D11SamplerState* sampler[] = { states->LinearClamp() };
	context->PSSetSamplers(0, 1, sampler);

	// �A���t�@�u�����h�̐ݒ�
	context->OMSetBlendState(states->AlphaBlend(), nullptr, 0xffffffff);

	// �[�x�X�e���V���X�e�[�g�̐ݒ�
	context->OMSetDepthStencilState(m_depthStencilState_Shadow.Get(), 1);

	// �e�̒��_���
	VertexPositionTexture vertexes[] =
	{
		VertexPositionTexture(SimpleMath::Vector3::Zero, SimpleMath::Vector2(0.0f, 0.0f)),
		VertexPositionTexture(SimpleMath::Vector3::Zero, SimpleMath::Vector2(1.0f, 0.0f)),
		VertexPositionTexture(SimpleMath::Vector3::Zero, SimpleMath::Vector2(0.0f, 1.0f)),
		VertexPositionTexture(SimpleMath::Vector3::Zero, SimpleMath::Vector2(1.0f, 1.0f)),
	};
	// �e�̃C���f�b�N�X���
	uint16_t indexes[] = { 0, 1, 2, 1, 3, 2 };

	// �e�̕\���ʒu�̐ݒ�
	vertexes[0].position = SimpleMath::Vector3(-radius, 0.01f, -radius) + position;
	vertexes[1].position = SimpleMath::Vector3( radius, 0.01f, -radius) + position;
	vertexes[2].position = SimpleMath::Vector3(-radius, 0.01f,  radius) + position;
	vertexes[3].position = SimpleMath::Vector3( radius, 0.01f,  radius) + position;

	// �e�̕`��
	m_primitiveBatch->Begin();
	m_primitiveBatch->DrawIndexed(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		indexes, _countof(indexes),
		vertexes, _countof(vertexes)
	);
	m_primitiveBatch->End();
}

