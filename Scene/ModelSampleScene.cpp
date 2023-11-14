#include "pch.h"
#include "ModelSampleScene.h"
#include "DebugDraw.h"
#include "ReadData.h"
#include <iomanip>

using namespace DirectX;

ModelSampleScene::ModelSampleScene()
{
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
		XMConvertToRadians(90.0f), 1.0f, 0.1f, 100.0f);
	
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

	// 床の描画
	m_floorModel->Draw(context, *states, world, m_view, m_proj, false, [&]()
		{
			ID3D11SamplerState* samplers[] = { states->PointWrap() };
			context->PSSetSamplers(0, 1, samplers);
		}
	);

	m_spriteBatch->Begin();

	m_spriteBatch->Draw(srv, SimpleMath::Vector2::Zero);

	m_spriteBatch->End();
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


