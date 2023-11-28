#include "pch.h"
#include "ModelSampleScene.h"
#include "DebugDraw.h"
#include "ReadData.h"
#include <iomanip>

using namespace DirectX;

ModelSampleScene::ModelSampleScene()
{
	// �X�|�b�g���C�g�͈̔͂̊p�x
	m_lightTheta = 90.0f;

	// ���C�g�̈ʒu
	m_lightPosition = SimpleMath::Vector3(5.0f, 5.0f, 0.0f);

	// ���C�g�̉�]
	m_lightRotate = SimpleMath::Quaternion::CreateFromYawPitchRoll(
		XMConvertToRadians(-90.0f), XMConvertToRadians(45.0f), 0.0f);
}

void ModelSampleScene::Initialize()
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();

	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();

	// �f�o�b�O�J�����̍쐬
	RECT rect = GetUserResources()->GetDeviceResources()->GetOutputSize();
	m_debugCamera = std::make_unique<Imase::DebugCamera>(rect.right, rect.bottom);

	// �萔�o�b�t�@�̓��e�X�V
	ConstantBuffer2 cb = {};
	cb.fCosTheta = cosf(XMConvertToRadians(m_lightTheta / 2.0f));
	context->UpdateSubresource(m_constantBuffer2.Get(), 0, nullptr, &cb, 0, 0);
}

void ModelSampleScene::Update(float elapsedTime)
{
	elapsedTime;

	auto kbTracker = GetUserResources()->GetKeyboardStateTracker();
	auto kb = kbTracker->GetLastState();
	auto mouseTracker = GetUserResources()->GetMouseStateTracker();
	auto mouseState = mouseTracker->GetLastState();

	// �f�o�b�O�J�����̍X�V
	m_debugCamera->Update(*mouseTracker);

	// �g�[���X���㉺������
	auto t = GetUserResources()->GetStepTimer()->GetTotalSeconds();
	m_torusPosition = SimpleMath::Vector3(1.0f, sinf(t) + 1.0f, 0.0f);

	// �X�y�[�X�L�[�Ń}�E�X�̃��[�h��؂�ւ���
	if (kbTracker->pressed.Space)
	{
		if (mouseState.positionMode == Mouse::Mode::MODE_ABSOLUTE)
		{
			Mouse::Get().SetMode(Mouse::Mode::MODE_RELATIVE);
		}
		else
		{
			Mouse::Get().SetMode(Mouse::Mode::MODE_ABSOLUTE);
		}
	}

	// �}�E�X�̃��[�h�����΃��[�h���H
	if (mouseState.positionMode == Mouse::Mode::MODE_RELATIVE)
	{
		// �}�E�X�̉E�{�^����������Ă�����X�|�b�g���C�g����]������
		if (mouseState.rightButton)
		{
			float yaw = -mouseState.x * 0.01f;
			float pitch = mouseState.y * 0.01f;
			SimpleMath::Quaternion q = SimpleMath::Quaternion::CreateFromYawPitchRoll(yaw, pitch, 0.0f);
			m_lightRotate = q * m_lightRotate;
		}
	}

}

void ModelSampleScene::Render()
{
	auto debugFont = GetUserResources()->GetDebugFont();
	debugFont->AddString(L"ModelSampleScene", SimpleMath::Vector2(0.0f, debugFont->GetFontHeight()));

	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
	auto states = GetUserResources()->GetCommonStates();

	// -------------------------------------------------------------------------- //
	// Pass1 �V���h�E�}�b�v�̍쐬
	// -------------------------------------------------------------------------- //

	auto rtv = m_shadowMapRT->GetRenderTargetView();
	auto srv = m_shadowMapRT->GetShaderResourceView();
	auto dsv = m_shadowMapDS->GetDepthStencilView();

	// �����_�[�^�[�Q�b�g��ύX�ishadowMapRT�j
	context->ClearRenderTargetView(rtv, SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f));
	context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);
	context->OMSetRenderTargets(1, &rtv, dsv);

	// �r���[�|�[�g��ݒ�
	D3D11_VIEWPORT vp = { 0.0f, 0.0f, SHADOWMAP_SIZE, SHADOWMAP_SIZE, 0.0f, 1.0f };
	context->RSSetViewports(1, &vp);

	m_view = m_debugCamera->GetCameraMatrix();

	// ------------------------------------------------ //
	// ���C�g��Ԃ̃r���[�s��Ǝˉe�s����쐬����
	// ------------------------------------------------ //

	// ���C�g�̕���
	SimpleMath::Vector3 lightDir = SimpleMath::Vector3::Transform(SimpleMath::Vector3(0.0f, 0.0f, 1.0f), m_lightRotate);

	// �r���[�s����쐬
	SimpleMath::Matrix view = SimpleMath::Matrix::CreateLookAt(
		m_lightPosition,
		m_lightPosition + lightDir,
		SimpleMath::Vector3::UnitY
	);

	// �ˉe�s����쐬
	SimpleMath::Matrix proj = SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		XMConvertToRadians(m_lightTheta), 1.0f, 0.1f, 100.0f);

	// -------------------------------------------------------------------------- //
	// �萔�o�b�t�@���X�V
	// -------------------------------------------------------------------------- //

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// GPU���萔�o�b�t�@�ɑ΂��ăA�N�Z�X���s��Ȃ��悤�ɂ���
	context->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// �萔�o�b�t�@���X�V
	ConstantBuffer cb = {};
	SimpleMath::Matrix m = view * proj;
	cb.lightViewProj = XMMatrixTranspose(m);
	cb.lightPosition = m_lightPosition;
	cb.lightDirection = lightDir;
	cb.lightAmbient = SimpleMath::Color(0.3f, 0.3f, 0.3f);

	*static_cast<ConstantBuffer*>(mappedResource.pData) = cb;

	// GPU���萔�o�b�t�@�ɑ΂��ẴA�N�Z�X��������
	context->Unmap(m_constantBuffer.Get(), 0);

	// ------------------------------------------------ //
	// �e�ɂȂ郂�f����`�悷��
	// ------------------------------------------------ //

	SimpleMath::Matrix world;

	world = SimpleMath::Matrix::CreateTranslation(0.0f, 1.0f, 0.0f);

	// �g�[���X�̕`��
	m_torusModel->Draw(context, *states, world, view, proj, false, [&]()
		{
			context->VSSetShader(m_VS_Depth.Get(), nullptr, 0);
			context->PSSetShader(m_PS_Depth.Get(), nullptr, 0);
		}
	);

	world = SimpleMath::Matrix::CreateTranslation(m_torusPosition);

	// �g�[���X�̕`��
	m_torusModel->Draw(context, *states, world, view, proj, false, [&]()
		{
			context->VSSetShader(m_VS_Depth.Get(), nullptr, 0);
			context->PSSetShader(m_PS_Depth.Get(), nullptr, 0);
		}
	);

	// -------------------------------------------------------------------------- //
	// �����_�[�^�[�Q�b�g�ƃr���[�|�[�g�����ɖ߂�
	// -------------------------------------------------------------------------- //
	auto renderTarget = GetUserResources()->GetDeviceResources()->GetRenderTargetView();
	auto depthStencil = GetUserResources()->GetDeviceResources()->GetDepthStencilView();

	context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);
	auto const viewport = GetUserResources()->GetDeviceResources()->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	world = SimpleMath::Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);

	// ���̕`��
	m_floorModel->Draw(context, *states, world, m_view, m_proj, false, [&]()
		{
			// �萔�o�b�t�@�̐ݒ�
			ID3D11Buffer* cbuf[] = { m_constantBuffer.Get(), m_constantBuffer2.Get() };
			context->VSSetConstantBuffers(1, 1, cbuf);
			context->PSSetConstantBuffers(1, 2, cbuf);

			// �쐬�����V���h�E�}�b�v�����\�[�X�Ƃ��Đݒ�
			context->PSSetShaderResources(1, 1, &srv);

			// �e�N�X�`���T���v���[�̐ݒ�
			ID3D11SamplerState* samplers[] = { states->PointWrap(), m_shadowMapSampler.Get()};
			context->PSSetSamplers(0, 2, samplers);

			// �V�F�[�_�[�̐ݒ�
			context->VSSetShader(m_VS.Get(), nullptr, 0);
			context->PSSetShader(m_PS.Get(), nullptr, 0);
		}
	);

	world = SimpleMath::Matrix::CreateTranslation(0.0f, 1.0f, 0.0f);

	// �g�[���X�̕`��
	m_torusModel->Draw(context, *states, world, m_view, m_proj, false, [&]()
		{
			// �萔�o�b�t�@�̐ݒ�
			ID3D11Buffer* cbuf[] = { m_constantBuffer.Get(), m_constantBuffer2.Get() };
			context->VSSetConstantBuffers(1, 1, cbuf);
			context->PSSetConstantBuffers(1, 2, cbuf);

			// �쐬�����V���h�E�}�b�v�����\�[�X�Ƃ��Đݒ�
			context->PSSetShaderResources(1, 1, &srv);

			// �e�N�X�`���T���v���[�̐ݒ�
			ID3D11SamplerState* samplers[] = { states->LinearWrap(), m_shadowMapSampler.Get() };
			context->PSSetSamplers(0, 2, samplers);

			// �V�F�[�_�[�̐ݒ�
			context->VSSetShader(m_VS.Get(), nullptr, 0);
			context->PSSetShader(m_PS.Get(), nullptr, 0);
		}
	);

	world = SimpleMath::Matrix::CreateTranslation(m_torusPosition);

	// �g�[���X�̕`��
	m_torusModel->Draw(context, *states, world, m_view, m_proj, false, [&]()
		{
			// �萔�o�b�t�@�̐ݒ�
			ID3D11Buffer* cbuf[] = { m_constantBuffer.Get(), m_constantBuffer2.Get() };
			context->VSSetConstantBuffers(1, 1, cbuf);
			context->PSSetConstantBuffers(1, 2, cbuf);

			// �쐬�����V���h�E�}�b�v�����\�[�X�Ƃ��Đݒ�
			context->PSSetShaderResources(1, 1, &srv);

			// �e�N�X�`���T���v���[�̐ݒ�
			ID3D11SamplerState* samplers[] = { states->LinearWrap(), m_shadowMapSampler.Get() };
			context->PSSetSamplers(0, 2, samplers);

			// �V�F�[�_�[�̐ݒ�
			context->VSSetShader(m_VS.Get(), nullptr, 0);
			context->PSSetShader(m_PS.Get(), nullptr, 0);
		}
	);

	// ���\�[�X�̊��蓖�Ă���������ishadowMapRT�j
	ID3D11ShaderResourceView* nullsrv[] = { nullptr };
	context->PSSetShaderResources(1, 1, nullsrv);

	world = SimpleMath::Matrix::CreateFromQuaternion(m_lightRotate) * SimpleMath::Matrix::CreateTranslation(m_lightPosition);

	// �X�|�b�g���C�g�̕`��
	m_spotLightModel->Draw(context, *states, world, m_view, m_proj);

	//m_spriteBatch->Begin();

	//m_spriteBatch->Draw(srv, SimpleMath::Vector2::Zero);

	//m_spriteBatch->End();
}

void ModelSampleScene::Finalize()
{
	//m_gridFloor.reset();
	m_floorModel.reset();
	m_torusModel.reset();
}

void ModelSampleScene::CreateDeviceDependentResources()
{
	auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();
	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
	//auto states = GetUserResources()->GetCommonStates();

	//// �O���b�h�̏����쐬
	//m_gridFloor = std::make_unique<Imase::GridFloor>(device, context, states);

	// �G�t�F�N�g�t�@�N�g���[�̍쐬
	EffectFactory fx(device);
	fx.SetDirectory(L"Resources/Models");

	// ���̃��f���̍쐬
	m_floorModel = Model::CreateFromCMO(device, L"Resources/Models/Floor.cmo", fx);

	// �g�[���X���f���̍쐬
	m_torusModel = Model::CreateFromCMO(device, L"Resources/Models/Torus.cmo", fx);

	// �X�|�b�g���C�g�̃��f���̍쐬
	m_spotLightModel = Model::CreateFromCMO(device, L"Resources/Models/SpotLight.cmo", fx);

	// ���Ȕ�������悤�ɃG�t�F�N�g��ݒ肷��
	m_spotLightModel->UpdateEffects([&](IEffect* effect)
		{
			auto basicEffect = dynamic_cast<BasicEffect*>(effect);
			if (basicEffect)
			{
				basicEffect->SetEmissiveColor(Colors::White);
			}
		}
	);

	//// �s�N�Z���V�F�[�_�[�̍쐬�i�g�[���X�p�j
	//std::vector<uint8_t> ps_torus = DX::ReadData(L"Resources/Shaders/PS_Test.cso");
	//DX::ThrowIfFailed(
	//	device->CreatePixelShader(ps_torus.data(), ps_torus.size(), nullptr, m_PS_Torus.ReleaseAndGetAddressOf())
	//);

	//// �萔�o�b�t�@�̍쐬
	//D3D11_BUFFER_DESC bufferDesc = {};
	//bufferDesc.ByteWidth = static_cast<UINT>(sizeof(ConstantBuffer));	// �m�ۂ���T�C�Y�i16�̔{���Őݒ肷��j
	//// GPU (�ǂݎ���p) �� CPU (�������ݐ�p) �̗�������A�N�Z�X�ł��郊�\�[�X
	//bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	//bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// �萔�o�b�t�@�Ƃ��Ĉ���
	//bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPU�����e��ύX�ł���悤�ɂ���
	//DX::ThrowIfFailed(device->CreateBuffer(&bufferDesc, nullptr, m_constantBuffer.ReleaseAndGetAddressOf()));

	RECT rect = { 0, 0, SHADOWMAP_SIZE, SHADOWMAP_SIZE };

	// �����_�[�e�N�X�`���̍쐬�i�V���h�E�}�b�v�p�j
	m_shadowMapRT = std::make_unique<DX::RenderTexture>(DXGI_FORMAT_R32_FLOAT);
	m_shadowMapRT->SetDevice(device);
	m_shadowMapRT->SetWindow(rect);

	// �f�v�X�X�e���V���̍쐬�i�V���h�E�}�b�v�p�j
	m_shadowMapDS = std::make_unique<Imase::DepthStencil>(DXGI_FORMAT_D32_FLOAT);
	m_shadowMapDS->SetDevice(device);
	m_shadowMapDS->SetWindow(rect);

	// �X�v���C�g�o�b�`�̍쐬
	m_spriteBatch = std::make_unique<SpriteBatch>(context);

	// ���_�V�F�[�_�[�̍쐬�i�V���h�E�}�b�v�p�j
	std::vector<uint8_t> vs_depth = DX::ReadData(L"Resources/Shaders/SM_VS_Depth.cso");
	DX::ThrowIfFailed(
		device->CreateVertexShader(vs_depth.data(), vs_depth.size(), nullptr, m_VS_Depth.ReleaseAndGetAddressOf())
	);

	// �s�N�Z���V�F�[�_�[�̍쐬�i�V���h�E�}�b�v�p�j
	std::vector<uint8_t> ps_depth = DX::ReadData(L"Resources/Shaders/SM_PS_Depth.cso");
	DX::ThrowIfFailed(
		device->CreatePixelShader(ps_depth.data(), ps_depth.size(), nullptr, m_PS_Depth.ReleaseAndGetAddressOf())
	);

	// �萔�o�b�t�@�̍쐬
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(ConstantBuffer));	// �m�ۂ���T�C�Y�i16�̔{���Őݒ肷��j
	// GPU (�ǂݎ���p) �� CPU (�������ݐ�p) �̗�������A�N�Z�X�ł��郊�\�[�X
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// �萔�o�b�t�@�Ƃ��Ĉ���
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPU�����e��ύX�ł���悤�ɂ���
	DX::ThrowIfFailed(device->CreateBuffer(&bufferDesc, nullptr, m_constantBuffer.ReleaseAndGetAddressOf()));

	// �萔�o�b�t�@�̍쐬
	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(ConstantBuffer2));	// �m�ۂ���T�C�Y�i16�̔{���Őݒ肷��j
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;	// GPU�̓ǂݎ��Ə������݂��\�Ȉ�ʓI�ȃ��\�[�X
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// �萔�o�b�t�@�Ƃ��Ĉ���
	bufferDesc.CPUAccessFlags = 0;	// CPU�̓A�N�Z�X���Ȃ��̂�0
	DX::ThrowIfFailed(device->CreateBuffer(&bufferDesc, nullptr, m_constantBuffer2.ReleaseAndGetAddressOf()));

	// ���_�V�F�[�_�[�̍쐬�i�V���h�E�}�b�v�p�j
	std::vector<uint8_t> vs = DX::ReadData(L"Resources/Shaders/SM_VS.cso");
	DX::ThrowIfFailed(
		device->CreateVertexShader(vs.data(), vs.size(), nullptr, m_VS.ReleaseAndGetAddressOf())
	);

	// �s�N�Z���V�F�[�_�[�̍쐬�i�V���h�E�}�b�v�p�j
	std::vector<uint8_t> ps = DX::ReadData(L"Resources/Shaders/SM_PS.cso");
	DX::ThrowIfFailed(
		device->CreatePixelShader(ps.data(), ps.size(), nullptr, m_PS.ReleaseAndGetAddressOf())
	);

	// �T���v���[�̍쐬�i�V���h�E�}�b�v�p�j
	D3D11_SAMPLER_DESC sampler_desc = CD3D11_SAMPLER_DESC(D3D11_DEFAULT);
	sampler_desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_LESS;
	device->CreateSamplerState(&sampler_desc, m_shadowMapSampler.ReleaseAndGetAddressOf());

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


