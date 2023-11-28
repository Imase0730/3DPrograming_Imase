#include "pch.h"
#include "ModelSampleScene.h"
#include "DebugDraw.h"
#include "ReadData.h"
#include <iomanip>

using namespace DirectX;

ModelSampleScene::ModelSampleScene()
{
	// スポットライトの範囲の角度
	m_lightTheta = 90.0f;

	// ライトの位置
	m_lightPosition = SimpleMath::Vector3(5.0f, 5.0f, 0.0f);

	// ライトの回転
	m_lightRotate = SimpleMath::Quaternion::CreateFromYawPitchRoll(
		XMConvertToRadians(-90.0f), XMConvertToRadians(45.0f), 0.0f);
}

void ModelSampleScene::Initialize()
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();

	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();

	// デバッグカメラの作成
	RECT rect = GetUserResources()->GetDeviceResources()->GetOutputSize();
	m_debugCamera = std::make_unique<Imase::DebugCamera>(rect.right, rect.bottom);

	// 定数バッファの内容更新
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

	// デバッグカメラの更新
	m_debugCamera->Update(*mouseTracker);

	// トーラスを上下させる
	auto t = GetUserResources()->GetStepTimer()->GetTotalSeconds();
	m_torusPosition = SimpleMath::Vector3(1.0f, sinf(t) + 1.0f, 0.0f);

	// スペースキーでマウスのモードを切り替える
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

	// マウスのモードが相対モードか？
	if (mouseState.positionMode == Mouse::Mode::MODE_RELATIVE)
	{
		// マウスの右ボタンが押されていたらスポットライトを回転させる
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
	// Pass1 シャドウマップの作成
	// -------------------------------------------------------------------------- //

	auto rtv = m_shadowMapRT->GetRenderTargetView();
	auto srv = m_shadowMapRT->GetShaderResourceView();
	auto dsv = m_shadowMapDS->GetDepthStencilView();

	// レンダーターゲットを変更（shadowMapRT）
	context->ClearRenderTargetView(rtv, SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f));
	context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);
	context->OMSetRenderTargets(1, &rtv, dsv);

	// ビューポートを設定
	D3D11_VIEWPORT vp = { 0.0f, 0.0f, SHADOWMAP_SIZE, SHADOWMAP_SIZE, 0.0f, 1.0f };
	context->RSSetViewports(1, &vp);

	m_view = m_debugCamera->GetCameraMatrix();

	// ------------------------------------------------ //
	// ライト空間のビュー行列と射影行列を作成する
	// ------------------------------------------------ //

	// ライトの方向
	SimpleMath::Vector3 lightDir = SimpleMath::Vector3::Transform(SimpleMath::Vector3(0.0f, 0.0f, 1.0f), m_lightRotate);

	// ビュー行列を作成
	SimpleMath::Matrix view = SimpleMath::Matrix::CreateLookAt(
		m_lightPosition,
		m_lightPosition + lightDir,
		SimpleMath::Vector3::UnitY
	);

	// 射影行列を作成
	SimpleMath::Matrix proj = SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		XMConvertToRadians(m_lightTheta), 1.0f, 0.1f, 100.0f);

	// -------------------------------------------------------------------------- //
	// 定数バッファを更新
	// -------------------------------------------------------------------------- //

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// GPUが定数バッファに対してアクセスを行わないようにする
	context->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// 定数バッファを更新
	ConstantBuffer cb = {};
	SimpleMath::Matrix m = view * proj;
	cb.lightViewProj = XMMatrixTranspose(m);
	cb.lightPosition = m_lightPosition;
	cb.lightDirection = lightDir;
	cb.lightAmbient = SimpleMath::Color(0.3f, 0.3f, 0.3f);

	*static_cast<ConstantBuffer*>(mappedResource.pData) = cb;

	// GPUが定数バッファに対してのアクセスを許可する
	context->Unmap(m_constantBuffer.Get(), 0);

	// ------------------------------------------------ //
	// 影になるモデルを描画する
	// ------------------------------------------------ //

	SimpleMath::Matrix world;

	world = SimpleMath::Matrix::CreateTranslation(0.0f, 1.0f, 0.0f);

	// トーラスの描画
	m_torusModel->Draw(context, *states, world, view, proj, false, [&]()
		{
			context->VSSetShader(m_VS_Depth.Get(), nullptr, 0);
			context->PSSetShader(m_PS_Depth.Get(), nullptr, 0);
		}
	);

	world = SimpleMath::Matrix::CreateTranslation(m_torusPosition);

	// トーラスの描画
	m_torusModel->Draw(context, *states, world, view, proj, false, [&]()
		{
			context->VSSetShader(m_VS_Depth.Get(), nullptr, 0);
			context->PSSetShader(m_PS_Depth.Get(), nullptr, 0);
		}
	);

	// -------------------------------------------------------------------------- //
	// レンダーターゲットとビューポートを元に戻す
	// -------------------------------------------------------------------------- //
	auto renderTarget = GetUserResources()->GetDeviceResources()->GetRenderTargetView();
	auto depthStencil = GetUserResources()->GetDeviceResources()->GetDepthStencilView();

	context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);
	auto const viewport = GetUserResources()->GetDeviceResources()->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	world = SimpleMath::Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);

	// 床の描画
	m_floorModel->Draw(context, *states, world, m_view, m_proj, false, [&]()
		{
			// 定数バッファの設定
			ID3D11Buffer* cbuf[] = { m_constantBuffer.Get(), m_constantBuffer2.Get() };
			context->VSSetConstantBuffers(1, 1, cbuf);
			context->PSSetConstantBuffers(1, 2, cbuf);

			// 作成したシャドウマップをリソースとして設定
			context->PSSetShaderResources(1, 1, &srv);

			// テクスチャサンプラーの設定
			ID3D11SamplerState* samplers[] = { states->PointWrap(), m_shadowMapSampler.Get()};
			context->PSSetSamplers(0, 2, samplers);

			// シェーダーの設定
			context->VSSetShader(m_VS.Get(), nullptr, 0);
			context->PSSetShader(m_PS.Get(), nullptr, 0);
		}
	);

	world = SimpleMath::Matrix::CreateTranslation(0.0f, 1.0f, 0.0f);

	// トーラスの描画
	m_torusModel->Draw(context, *states, world, m_view, m_proj, false, [&]()
		{
			// 定数バッファの設定
			ID3D11Buffer* cbuf[] = { m_constantBuffer.Get(), m_constantBuffer2.Get() };
			context->VSSetConstantBuffers(1, 1, cbuf);
			context->PSSetConstantBuffers(1, 2, cbuf);

			// 作成したシャドウマップをリソースとして設定
			context->PSSetShaderResources(1, 1, &srv);

			// テクスチャサンプラーの設定
			ID3D11SamplerState* samplers[] = { states->LinearWrap(), m_shadowMapSampler.Get() };
			context->PSSetSamplers(0, 2, samplers);

			// シェーダーの設定
			context->VSSetShader(m_VS.Get(), nullptr, 0);
			context->PSSetShader(m_PS.Get(), nullptr, 0);
		}
	);

	world = SimpleMath::Matrix::CreateTranslation(m_torusPosition);

	// トーラスの描画
	m_torusModel->Draw(context, *states, world, m_view, m_proj, false, [&]()
		{
			// 定数バッファの設定
			ID3D11Buffer* cbuf[] = { m_constantBuffer.Get(), m_constantBuffer2.Get() };
			context->VSSetConstantBuffers(1, 1, cbuf);
			context->PSSetConstantBuffers(1, 2, cbuf);

			// 作成したシャドウマップをリソースとして設定
			context->PSSetShaderResources(1, 1, &srv);

			// テクスチャサンプラーの設定
			ID3D11SamplerState* samplers[] = { states->LinearWrap(), m_shadowMapSampler.Get() };
			context->PSSetSamplers(0, 2, samplers);

			// シェーダーの設定
			context->VSSetShader(m_VS.Get(), nullptr, 0);
			context->PSSetShader(m_PS.Get(), nullptr, 0);
		}
	);

	// リソースの割り当てを解除する（shadowMapRT）
	ID3D11ShaderResourceView* nullsrv[] = { nullptr };
	context->PSSetShaderResources(1, 1, nullsrv);

	world = SimpleMath::Matrix::CreateFromQuaternion(m_lightRotate) * SimpleMath::Matrix::CreateTranslation(m_lightPosition);

	// スポットライトの描画
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

	//// グリッドの床を作成
	//m_gridFloor = std::make_unique<Imase::GridFloor>(device, context, states);

	// エフェクトファクトリーの作成
	EffectFactory fx(device);
	fx.SetDirectory(L"Resources/Models");

	// 床のモデルの作成
	m_floorModel = Model::CreateFromCMO(device, L"Resources/Models/Floor.cmo", fx);

	// トーラスモデルの作成
	m_torusModel = Model::CreateFromCMO(device, L"Resources/Models/Torus.cmo", fx);

	// スポットライトのモデルの作成
	m_spotLightModel = Model::CreateFromCMO(device, L"Resources/Models/SpotLight.cmo", fx);

	// 自己発光するようにエフェクトを設定する
	m_spotLightModel->UpdateEffects([&](IEffect* effect)
		{
			auto basicEffect = dynamic_cast<BasicEffect*>(effect);
			if (basicEffect)
			{
				basicEffect->SetEmissiveColor(Colors::White);
			}
		}
	);

	//// ピクセルシェーダーの作成（トーラス用）
	//std::vector<uint8_t> ps_torus = DX::ReadData(L"Resources/Shaders/PS_Test.cso");
	//DX::ThrowIfFailed(
	//	device->CreatePixelShader(ps_torus.data(), ps_torus.size(), nullptr, m_PS_Torus.ReleaseAndGetAddressOf())
	//);

	//// 定数バッファの作成
	//D3D11_BUFFER_DESC bufferDesc = {};
	//bufferDesc.ByteWidth = static_cast<UINT>(sizeof(ConstantBuffer));	// 確保するサイズ（16の倍数で設定する）
	//// GPU (読み取り専用) と CPU (書き込み専用) の両方からアクセスできるリソース
	//bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	//bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// 定数バッファとして扱う
	//bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPUが内容を変更できるようにする
	//DX::ThrowIfFailed(device->CreateBuffer(&bufferDesc, nullptr, m_constantBuffer.ReleaseAndGetAddressOf()));

	RECT rect = { 0, 0, SHADOWMAP_SIZE, SHADOWMAP_SIZE };

	// レンダーテクスチャの作成（シャドウマップ用）
	m_shadowMapRT = std::make_unique<DX::RenderTexture>(DXGI_FORMAT_R32_FLOAT);
	m_shadowMapRT->SetDevice(device);
	m_shadowMapRT->SetWindow(rect);

	// デプスステンシルの作成（シャドウマップ用）
	m_shadowMapDS = std::make_unique<Imase::DepthStencil>(DXGI_FORMAT_D32_FLOAT);
	m_shadowMapDS->SetDevice(device);
	m_shadowMapDS->SetWindow(rect);

	// スプライトバッチの作成
	m_spriteBatch = std::make_unique<SpriteBatch>(context);

	// 頂点シェーダーの作成（シャドウマップ用）
	std::vector<uint8_t> vs_depth = DX::ReadData(L"Resources/Shaders/SM_VS_Depth.cso");
	DX::ThrowIfFailed(
		device->CreateVertexShader(vs_depth.data(), vs_depth.size(), nullptr, m_VS_Depth.ReleaseAndGetAddressOf())
	);

	// ピクセルシェーダーの作成（シャドウマップ用）
	std::vector<uint8_t> ps_depth = DX::ReadData(L"Resources/Shaders/SM_PS_Depth.cso");
	DX::ThrowIfFailed(
		device->CreatePixelShader(ps_depth.data(), ps_depth.size(), nullptr, m_PS_Depth.ReleaseAndGetAddressOf())
	);

	// 定数バッファの作成
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(ConstantBuffer));	// 確保するサイズ（16の倍数で設定する）
	// GPU (読み取り専用) と CPU (書き込み専用) の両方からアクセスできるリソース
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// 定数バッファとして扱う
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPUが内容を変更できるようにする
	DX::ThrowIfFailed(device->CreateBuffer(&bufferDesc, nullptr, m_constantBuffer.ReleaseAndGetAddressOf()));

	// 定数バッファの作成
	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(ConstantBuffer2));	// 確保するサイズ（16の倍数で設定する）
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;	// GPUの読み取りと書き込みが可能な一般的なリソース
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// 定数バッファとして扱う
	bufferDesc.CPUAccessFlags = 0;	// CPUはアクセスしないので0
	DX::ThrowIfFailed(device->CreateBuffer(&bufferDesc, nullptr, m_constantBuffer2.ReleaseAndGetAddressOf()));

	// 頂点シェーダーの作成（シャドウマップ用）
	std::vector<uint8_t> vs = DX::ReadData(L"Resources/Shaders/SM_VS.cso");
	DX::ThrowIfFailed(
		device->CreateVertexShader(vs.data(), vs.size(), nullptr, m_VS.ReleaseAndGetAddressOf())
	);

	// ピクセルシェーダーの作成（シャドウマップ用）
	std::vector<uint8_t> ps = DX::ReadData(L"Resources/Shaders/SM_PS.cso");
	DX::ThrowIfFailed(
		device->CreatePixelShader(ps.data(), ps.size(), nullptr, m_PS.ReleaseAndGetAddressOf())
	);

	// サンプラーの作成（シャドウマップ用）
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


