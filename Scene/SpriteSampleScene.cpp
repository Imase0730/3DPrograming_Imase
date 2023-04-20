#include "../pch.h"
#include "SpriteSampleScene.h"

using namespace DirectX;

void SpriteSampleScene::Initialize()
{
}

void SpriteSampleScene::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);
}

void SpriteSampleScene::Render()
{
	auto* debugFont = GetUserResources()->GetDebugFont();
	debugFont->AddString(L"SpriteSampleScene", SimpleMath::Vector2(0.0f, debugFont->GetFontHeight()));
}

void SpriteSampleScene::Finalize()
{
}

void SpriteSampleScene::CreateDeviceDependentResources()
{
}

void SpriteSampleScene::CreateWindowSizeDependentResources()
{
}

void SpriteSampleScene::OnDeviceLost()
{
}
