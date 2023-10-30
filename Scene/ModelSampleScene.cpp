#include "pch.h"
#include "ModelSampleScene.h"
#include "DebugDraw.h"
#include "ReadData.h"
#include <iomanip>

using namespace DirectX;

ModelSampleScene::ModelSampleScene()
	: m_fireFlag(false), m_distance(0.0f)
{
	m_cursor = 0;
	m_att[0] = 0.0f;
	m_att[1] = 0.0f;
	m_att[2] = 1.0f;
	m_lightPosition = SimpleMath::Vector3::Zero;
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

	// ���̌����W���̐��l���L�[�ŕύX�i�����p�j
	if (kbTracker->pressed.Down)
	{
		m_cursor = (m_cursor + 1) % 3;
	}
	if (kbTracker->pressed.Up)
	{
		m_cursor = (m_cursor + 2) % 3;
	}
	if (kb.Left)
	{
		m_att[m_cursor] -= 0.01f;
		if (m_att[m_cursor] < 0.0f) m_att[m_cursor] = 0.0f;
	}
	if (kb.Right)
	{
		m_att[m_cursor] += 0.01f;
	}

	// ���̌����W����\��
	for (int i = 0; i < 3; i++)
	{
		std::wostringstream oss;
		if (m_cursor == i)
		{
			oss << L">";
		}
		else
		{
			oss << L" ";
		}
		oss << L" att" << i << L" : " << std::fixed << std::setprecision(2) << m_att[i];
		GetUserResources()->GetDebugFont()->AddString(oss.str().c_str(), SimpleMath::Vector2(0, GetUserResources()->GetDebugFont()->GetFontHeight() * (i + 3)));
	}

	// ���C�g�̈ʒu���ړ�������
	float time = static_cast<float>(GetUserResources()->GetStepTimer()->GetTotalSeconds());
	m_lightPosition = SimpleMath::Vector3(sinf(time) * 2.0f, 1.0f, 0.0f);
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
	//m_gridFloor->Render(context, m_view, m_proj);

	// �g�[���X��`��
	SimpleMath::Matrix world;
	m_torusModel->Draw(context, *states, world, m_view, m_proj, false, [&]()
		{
			// �萔�o�b�t�@���X�V
			D3D11_MAPPED_SUBRESOURCE mappedResource;

			// GPU���萔�o�b�t�@�ɑ΂��ăA�N�Z�X���s��Ȃ��悤�ɂ���
			context->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

			// �萔�o�b�t�@���X�V
			ConstantBuffer cb = {};
			cb.att0 = m_att[0];
			cb.att1 = m_att[1];
			cb.att2 = m_att[2];
			cb.lightPosition = m_lightPosition;

			*static_cast<ConstantBuffer*>(mappedResource.pData) = cb;

			// GPU���萔�o�b�t�@�ɑ΂��ẴA�N�Z�X��������
			context->Unmap(m_constantBuffer.Get(), 0);

			// �s�N�Z���V�F�[�_�g�p����萔�o�b�t�@��ݒ�
			ID3D11Buffer* cbuf_ps[] = { m_constantBuffer.Get() };
			context->PSSetConstantBuffers(1, 1, cbuf_ps);	// �X���b�g�O��DirectXTK���g�p���Ă���̂ŃX���b�g�P���g�p����

			// �I���W�i���s�N�Z���V�F�[�_�[�̐ݒ�
			context->PSSetShader(m_PS_Torus.Get(), nullptr, 0);
		}
	);

	// ���̕`��
	m_floorModel->Draw(context, *states, world, m_view, m_proj, false, [&]()
		{
			ID3D11SamplerState* samplers[] = { states->PointWrap() };
			context->PSSetSamplers(0, 1, samplers);

			// �I���W�i���s�N�Z���V�F�[�_�[�̐ݒ�
			context->PSSetShader(m_PS_Torus.Get(), nullptr, 0);
		}
	);

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
	// ���̃��f���̃G�t�F�N�g��ݒ肷��
	m_floorModel->UpdateEffects([&](IEffect* effect)
		{
			auto basicEffect = dynamic_cast<BasicEffect*>(effect);
			if (basicEffect)
			{
				basicEffect->SetLightingEnabled(true);
				basicEffect->SetPerPixelLighting(true);
				basicEffect->SetTextureEnabled(true);
				basicEffect->SetVertexColorEnabled(false);
				basicEffect->SetFogEnabled(false);
			}
		}
	);

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

	// �g�[���X���f���̍쐬
	m_torusModel = Model::CreateFromCMO(device, L"Resources/Models/Torus.cmo", fx);
	// �g�[���X���f���̃G�t�F�N�g��ݒ肷��
	m_torusModel->UpdateEffects([&](IEffect* effect)
		{
			auto basicEffect = dynamic_cast<BasicEffect*>(effect);
			if (basicEffect)
			{
				basicEffect->SetLightingEnabled(true);
				basicEffect->SetPerPixelLighting(true);
				basicEffect->SetTextureEnabled(true);
				basicEffect->SetVertexColorEnabled(false);
				basicEffect->SetFogEnabled(false);
			}
		}
	);

	// �s�N�Z���V�F�[�_�[�̍쐬�i�g�[���X�p�j
	std::vector<uint8_t> ps_torus = DX::ReadData(L"Resources/Shaders/PS_Test.cso");
	DX::ThrowIfFailed(
		device->CreatePixelShader(ps_torus.data(), ps_torus.size(), nullptr, m_PS_Torus.ReleaseAndGetAddressOf())
	);

	// �萔�o�b�t�@�̍쐬
	D3D11_BUFFER_DESC bufferDesc = {};
	//int size = sizeof(ConstantBuffer) / 16;
	//if (sizeof(ConstantBuffer) % 16) size++;
	//bufferDesc.ByteWidth = static_cast<UINT>(size * 16);	// �m�ۂ���T�C�Y�i16�̔{���Őݒ肷��j
	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(ConstantBuffer));	// �m�ۂ���T�C�Y�i16�̔{���Őݒ肷��j
	// GPU (�ǂݎ���p) �� CPU (�������ݐ�p) �̗�������A�N�Z�X�ł��郊�\�[�X
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// �萔�o�b�t�@�Ƃ��Ĉ���
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPU�����e��ύX�ł���悤�ɂ���
	DX::ThrowIfFailed(device->CreateBuffer(&bufferDesc, nullptr, m_constantBuffer.ReleaseAndGetAddressOf()));
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

