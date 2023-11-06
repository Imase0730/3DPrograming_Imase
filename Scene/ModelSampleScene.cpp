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

	// デバッグカメラの作成
	RECT rect = GetUserResources()->GetDeviceResources()->GetOutputSize();
	m_debugCamera = std::make_unique<Imase::DebugCamera>(rect.right, rect.bottom);
}

void ModelSampleScene::Update(float elapsedTime)
{
	elapsedTime;

	auto kb = Keyboard::Get().GetState();
	auto kbTracker = GetUserResources()->GetKeyboardStateTracker();

	// デバッグカメラの更新
	m_debugCamera->Update();

	// 光の減衰係数の数値をキーで変更（調整用）
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

	// 光の減衰係数を表示
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

	// ライトの位置を移動させる
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
	// レンダーターゲットを変更（sceneRT）
	// -------------------------------------------------------------------------- //
	context->ClearRenderTargetView(m_offscreenRT->GetRenderTargetView(), Colors::Black);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &offscreenRTV, depthStencil);
	// -------------------------------------------------------------------------- //

	// グリッドの床を描画
	//m_gridFloor->Render(context, m_view, m_proj);

	// 画面のサイズを取得
	RECT rect = GetUserResources()->GetDeviceResources()->GetOutputSize();

	// ビュー行列を設定
	m_view = m_debugCamera->GetCameraMatrix();

	// 左上のシーンの描画
	D3D11_VIEWPORT vp = {};
	vp.Width = rect.right / 2.0f;
	vp.Height = rect.bottom / 2.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	DrawScene(vp, m_view);

	// 右上のシーンの描画
	vp.TopLeftX = rect.right / 2.0f;
	vp.TopLeftY = 0.0f;
	m_view = SimpleMath::Matrix::CreateLookAt(SimpleMath::Vector3(0.0f, 4.0f, 5.0f), SimpleMath::Vector3(0.0f, 0.0f, 0.0f), SimpleMath::Vector3::UnitY);
	DrawScene(vp, m_view);

	// 左下のシーンの描画
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = rect.bottom / 2.0f;
	m_view = SimpleMath::Matrix::CreateLookAt(SimpleMath::Vector3(5.0f, 10.0f, 5.0f), SimpleMath::Vector3(0.0f, 0.0f, 0.0f), SimpleMath::Vector3::UnitY);
	DrawScene(vp, m_view);

	// 右下のシーンの描画
	vp.TopLeftX = rect.right / 2.0f;
	vp.TopLeftY = rect.bottom / 2.0f;
	m_view = SimpleMath::Matrix::CreateLookAt(SimpleMath::Vector3(0.0f, 2.0f, 2.0f), SimpleMath::Vector3(0.0f, 0.0f, 0.0f), SimpleMath::Vector3::UnitY);
	DrawScene(vp, m_view);

	// -------------------------------------------------------------------------- //
	// レンダーターゲットとビューポートを元に戻す
	// -------------------------------------------------------------------------- //
	context->ClearRenderTargetView(renderTarget, Colors::Black);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);
	auto const viewport = GetUserResources()->GetDeviceResources()->GetScreenViewport();
	context->RSSetViewports(1, &viewport);
	// -------------------------------------------------------------------------- //

	// セピア調に色を変えるポストプロセスを実行する
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

	//// グリッドの床を作成
	//m_gridFloor = std::make_unique<Imase::GridFloor>(device, context, states);

	// エフェクトファクトリーの作成
	EffectFactory fx(device);
	fx.SetDirectory(L"Resources/Models");

	// 床のモデルの作成
	m_floorModel = Model::CreateFromCMO(device, L"Resources/Models/Floor.cmo", fx);
	// 床のモデルのエフェクトを設定する
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

	// トーラスモデルの作成
	m_torusModel = Model::CreateFromCMO(device, L"Resources/Models/Torus.cmo", fx);
	// トーラスモデルのエフェクトを設定する
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

	// ピクセルシェーダーの作成（トーラス用）
	std::vector<uint8_t> ps_torus = DX::ReadData(L"Resources/Shaders/PS_Test.cso");
	DX::ThrowIfFailed(
		device->CreatePixelShader(ps_torus.data(), ps_torus.size(), nullptr, m_PS_Torus.ReleaseAndGetAddressOf())
	);

	// 定数バッファの作成
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(ConstantBuffer));	// 確保するサイズ（16の倍数で設定する）
	// GPU (読み取り専用) と CPU (書き込み専用) の両方からアクセスできるリソース
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// 定数バッファとして扱う
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPUが内容を変更できるようにする
	DX::ThrowIfFailed(device->CreateBuffer(&bufferDesc, nullptr, m_constantBuffer.ReleaseAndGetAddressOf()));

	// レンダーテクスチャの作成（シーン全体）
	m_offscreenRT = std::make_unique<DX::RenderTexture>(DXGI_FORMAT_B8G8R8A8_UNORM);
	m_offscreenRT->SetDevice(device);
	RECT rect = GetUserResources()->GetDeviceResources()->GetOutputSize();
	m_offscreenRT->SetWindow(rect);

	// スプライトバッチの作成
	m_spriteBatch = std::make_unique<SpriteBatch>(context);

	// ベーシックポストプロセス
	m_basicPostProcess = std::make_unique<BasicPostProcess>(device);
}

void ModelSampleScene::CreateWindowSizeDependentResources()
{
	// 射影行列を作成
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

// シーンの描画
void ModelSampleScene::DrawScene(const D3D11_VIEWPORT& vp, const DirectX::SimpleMath::Matrix& view)
{
	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
	auto states = GetUserResources()->GetCommonStates();

	// ビューポートの設定
	context->RSSetViewports(1, &vp);

	// トーラスを描画
	SimpleMath::Matrix world;
	m_torusModel->Draw(context, *states, world, view, m_proj, false, [&]()
		{
			// 定数バッファを更新
			D3D11_MAPPED_SUBRESOURCE mappedResource;

			// GPUが定数バッファに対してアクセスを行わないようにする
			context->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

			// 定数バッファを更新
			ConstantBuffer cb = {};
			cb.att0 = m_att[0];
			cb.att1 = m_att[1];
			cb.att2 = m_att[2];
			cb.lightPosition = m_lightPosition;

			*static_cast<ConstantBuffer*>(mappedResource.pData) = cb;

			// GPUが定数バッファに対してのアクセスを許可する
			context->Unmap(m_constantBuffer.Get(), 0);

			// ピクセルシェーダ使用する定数バッファを設定
			ID3D11Buffer* cbuf_ps[] = { m_constantBuffer.Get() };
			context->PSSetConstantBuffers(1, 1, cbuf_ps);	// スロット０はDirectXTKが使用しているのでスロット１を使用する

			// オリジナルピクセルシェーダーの設定
			context->PSSetShader(m_PS_Torus.Get(), nullptr, 0);
		}
	);

	// 床の描画
	m_floorModel->Draw(context, *states, world, view, m_proj, false, [&]()
		{
			ID3D11SamplerState* samplers[] = { states->PointWrap() };
			context->PSSetSamplers(0, 1, samplers);

			// オリジナルピクセルシェーダーの設定
			context->PSSetShader(m_PS_Torus.Get(), nullptr, 0);
		}
	);
}

