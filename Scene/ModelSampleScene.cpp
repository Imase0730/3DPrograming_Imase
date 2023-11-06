#include "pch.h"
#include "ModelSampleScene.h"
#include "DebugDraw.h"
#include "ReadData.h"
#include <iomanip>

using namespace DirectX;

ModelSampleScene::ModelSampleScene()
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
}

void ModelSampleScene::Update(float elapsedTime)
{
	elapsedTime;

	auto kb = Keyboard::Get().GetState();
	auto kbTracker = GetUserResources()->GetKeyboardStateTracker();

	// �f�o�b�O�J�����̍X�V
	m_debugCamera->Update();

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

	auto renderTarget = GetUserResources()->GetDeviceResources()->GetRenderTargetView();
	auto depthStencil = GetUserResources()->GetDeviceResources()->GetDepthStencilView();
	auto offscreenRTV = m_offscreenRT->GetRenderTargetView();
	auto offscreenSRV = m_offscreenRT->GetShaderResourceView();

	// -------------------------------------------------------------------------- //
	// �����_�[�^�[�Q�b�g��ύX�isceneRT�j
	// -------------------------------------------------------------------------- //
	context->ClearRenderTargetView(m_offscreenRT->GetRenderTargetView(), Colors::Black);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &offscreenRTV, depthStencil);
	// -------------------------------------------------------------------------- //

	// �O���b�h�̏���`��
	//m_gridFloor->Render(context, m_view, m_proj);

	// ��ʂ̃T�C�Y���擾
	RECT rect = GetUserResources()->GetDeviceResources()->GetOutputSize();

	// �r���[�s���ݒ�
	m_view = m_debugCamera->GetCameraMatrix();

	// ����̃V�[���̕`��
	D3D11_VIEWPORT vp = {};
	vp.Width = rect.right / 2.0f;
	vp.Height = rect.bottom / 2.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	DrawScene(vp, m_view);

	// �E��̃V�[���̕`��
	vp.TopLeftX = rect.right / 2.0f;
	vp.TopLeftY = 0.0f;
	m_view = SimpleMath::Matrix::CreateLookAt(SimpleMath::Vector3(0.0f, 4.0f, 5.0f), SimpleMath::Vector3(0.0f, 0.0f, 0.0f), SimpleMath::Vector3::UnitY);
	DrawScene(vp, m_view);

	// �����̃V�[���̕`��
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = rect.bottom / 2.0f;
	m_view = SimpleMath::Matrix::CreateLookAt(SimpleMath::Vector3(5.0f, 10.0f, 5.0f), SimpleMath::Vector3(0.0f, 0.0f, 0.0f), SimpleMath::Vector3::UnitY);
	DrawScene(vp, m_view);

	// �E���̃V�[���̕`��
	vp.TopLeftX = rect.right / 2.0f;
	vp.TopLeftY = rect.bottom / 2.0f;
	m_view = SimpleMath::Matrix::CreateLookAt(SimpleMath::Vector3(0.0f, 2.0f, 2.0f), SimpleMath::Vector3(0.0f, 0.0f, 0.0f), SimpleMath::Vector3::UnitY);
	DrawScene(vp, m_view);

	// -------------------------------------------------------------------------- //
	// �����_�[�^�[�Q�b�g�ƃr���[�|�[�g�����ɖ߂�
	// -------------------------------------------------------------------------- //
	context->ClearRenderTargetView(renderTarget, Colors::Black);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);
	auto const viewport = GetUserResources()->GetDeviceResources()->GetScreenViewport();
	context->RSSetViewports(1, &viewport);
	// -------------------------------------------------------------------------- //

	// �Z�s�A���ɐF��ς���|�X�g�v���Z�X�����s����
	m_basicPostProcess->SetEffect(BasicPostProcess::Sepia);
	m_basicPostProcess->SetSourceTexture(offscreenSRV);
	m_basicPostProcess->Process(context);

	//m_spriteBatch->Begin();

	//m_spriteBatch->Draw(sceneSRV, SimpleMath::Vector2::Zero);

	//m_spriteBatch->End();
}

void ModelSampleScene::Finalize()
{
	//m_gridFloor.reset();
	m_floorModel.reset();
	m_torusModel.reset();
	m_constantBuffer.Reset();
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
	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(ConstantBuffer));	// �m�ۂ���T�C�Y�i16�̔{���Őݒ肷��j
	// GPU (�ǂݎ���p) �� CPU (�������ݐ�p) �̗�������A�N�Z�X�ł��郊�\�[�X
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// �萔�o�b�t�@�Ƃ��Ĉ���
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPU�����e��ύX�ł���悤�ɂ���
	DX::ThrowIfFailed(device->CreateBuffer(&bufferDesc, nullptr, m_constantBuffer.ReleaseAndGetAddressOf()));

	// �����_�[�e�N�X�`���̍쐬�i�V�[���S�́j
	m_offscreenRT = std::make_unique<DX::RenderTexture>(DXGI_FORMAT_B8G8R8A8_UNORM);
	m_offscreenRT->SetDevice(device);
	RECT rect = GetUserResources()->GetDeviceResources()->GetOutputSize();
	m_offscreenRT->SetWindow(rect);

	// �X�v���C�g�o�b�`�̍쐬
	m_spriteBatch = std::make_unique<SpriteBatch>(context);

	// �x�[�V�b�N�|�X�g�v���Z�X
	m_basicPostProcess = std::make_unique<BasicPostProcess>(device);
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

// �V�[���̕`��
void ModelSampleScene::DrawScene(const D3D11_VIEWPORT& vp, const DirectX::SimpleMath::Matrix& view)
{
	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
	auto states = GetUserResources()->GetCommonStates();

	// �r���[�|�[�g�̐ݒ�
	context->RSSetViewports(1, &vp);

	// �g�[���X��`��
	SimpleMath::Matrix world;
	m_torusModel->Draw(context, *states, world, view, m_proj, false, [&]()
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
	m_floorModel->Draw(context, *states, world, view, m_proj, false, [&]()
		{
			ID3D11SamplerState* samplers[] = { states->PointWrap() };
			context->PSSetSamplers(0, 1, samplers);

			// �I���W�i���s�N�Z���V�F�[�_�[�̐ݒ�
			context->PSSetShader(m_PS_Torus.Get(), nullptr, 0);
		}
	);
}

