//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include "Scene/SpriteSampleScene.h"
#include "Scene/ModelSampleScene.h"

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
    : m_fullscreen(FALSE)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>(
        DXGI_FORMAT_B8G8R8A8_UNORM,
        DXGI_FORMAT_D24_UNORM_S8_UINT   // ステンシルを使用するため
    );
    // TODO: Provide parameters for swapchain format, depth/stencil format, and backbuffer count.
    //   Add DX::DeviceResources::c_AllowTearing to opt-in to variable rate displays.
    //   Add DX::DeviceResources::c_EnableHDR for HDR10 display.
    m_deviceResources->RegisterDeviceNotify(this);

    // レンダリングテクスチャの作成
    m_transitionTexture = std::make_unique<DX::RenderTexture>(m_deviceResources->GetBackBufferFormat());
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */

    // 起動シーン設定
    m_sceneManager->SetScene<ModelSampleScene>();

    // BLANK（黒で塗りつぶす）
    auto context = m_deviceResources->GetD3DDeviceContext();
    context->ClearRenderTargetView(m_transitionTexture->GetRenderTargetView(), Colors::Black);
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

    // TODO: Add your game logic here.

    // キーボードステートトラッカーの更新
    m_keyboardTracker.Update(Keyboard::Get().GetState());

    // マウスステートトラッカーの更新
    m_mouseTracker.Update(Mouse::Get().GetState());

    // シーンの更新
    m_sceneManager->Update(elapsedTime);

    // 画面遷移マスクの更新
    m_transitionMask->Update(elapsedTime);
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();

    // TODO: Add your rendering code here.
    context;

    // シーンの描画
    m_sceneManager->Render();

    // フレームバッファのコピーリクエストがあるなら
    if (m_transitionMask->GetCreateMaskRequest() == TransitionMask::CreateMaskRequest::COPY)
    {
        // COPY（現在のフレームバッファをマスクに設定する）
        auto renderTarget = m_deviceResources->GetRenderTarget();
        context->CopyResource(m_transitionTexture->GetRenderTarget(), renderTarget);
        m_transitionMask->SetCreateMaskRequest(TransitionMask::CreateMaskRequest::NONE);
    }

    // 画面遷移マスクの描画
    m_transitionMask->Draw(context, m_states.get(), m_transitionTexture->GetShaderResourceView(), m_deviceResources->GetOutputSize());

    // fpsの表示
    std::wostringstream oss;
    oss << "fps:" << m_timer.GetFramesPerSecond();
    m_debugFont->AddString(oss.str().c_str(), SimpleMath::Vector2(0.0f, 0.0f));

    // デバッグ用文字列の描画
    m_debugFont->Render(m_states.get());

    m_deviceResources->PIXEndEvent();

    // Show the new frame.
    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto const viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
    auto const r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);

    // フルスクリーンか調べる
    BOOL fullscreen = FALSE;
    m_deviceResources->GetSwapChain()->GetFullscreenState(&fullscreen, nullptr);
    // フルスクリーンが解除されてしまった時の処理
    if (m_fullscreen != fullscreen)
    {
        m_fullscreen = fullscreen;
        // ResizeBuffers関数を呼び出す
        m_deviceResources->CreateWindowSizeDependentResources();
    }
}

void Game::OnDisplayChange()
{
    m_deviceResources->UpdateColorSpace();
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 1280;
    height = 720;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();
    auto context = m_deviceResources->GetD3DDeviceContext();

    // TODO: Initialize device dependent objects here (independent of window size).
     // 共通ステートオブジェクトの作成

    m_states = std::make_unique<CommonStates>(device);

    // デバッグ用文字列表示オブジェクトの作成
    m_debugFont = std::make_unique<Imase::DebugFont>(device, context, L"Resources/Font/SegoeUI_18.spritefont");

    // ユーザーリソースの作成
    if (!m_userResources) m_userResources = std::make_unique<UserResources>();

    // シーンマネージャーの作成
    if (!m_sceneManager) m_sceneManager = std::make_unique<Imase::SceneManager<UserResources>>(m_userResources.get());

    // 画面遷移マスクの作成
    m_transitionMask = std::make_unique<TransitionMask>(device, context);

    // シーンへ渡すユーザーリソースの設定（ここで設定してください）
    m_userResources->SetDeviceResources(m_deviceResources.get());
    m_userResources->SetCommonStates(m_states.get());
    m_userResources->SetDebugFont(m_debugFont.get());
    m_userResources->SetKeyboardStateTracker(&m_keyboardTracker);
    m_userResources->SetMouseStateTracker(&m_mouseTracker);
    m_userResources->SetStepTimerStates(&m_timer);
    m_userResources->SetTransitionMask(m_transitionMask.get());

    // 実行中のシーンのCreateDeviceDependentResources関数を呼び出す
    m_sceneManager->CreateDeviceDependentResources();
 
    // レンダリングテクスチャにデバイスを設定する
    m_transitionTexture->SetDevice(device);
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.

    // 実行中のシーンのCreateWindowSizeDependentResources関数を呼び出す
    m_sceneManager->CreateWindowSizeDependentResources();
 
    // レンダリングテクスチャのサイズを変更
    auto size = m_deviceResources->GetOutputSize();
    m_transitionTexture->SetWindow(size);
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.

    // 実行中のシーンのOnDeviceLost関数を呼び出す
    m_sceneManager->OnDeviceLost();

    // レンダリングテクスチャの解放
    m_transitionTexture->ReleaseDevice();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
