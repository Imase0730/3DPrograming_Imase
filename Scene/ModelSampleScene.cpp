#include "pch.h"
#include "ModelSampleScene.h"

using namespace DirectX;

ModelSampleScene::ModelSampleScene()
{
}

void ModelSampleScene::Initialize()
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

void ModelSampleScene::Update(float elapsedTime)
{
	elapsedTime;
}

void ModelSampleScene::Render()
{
	auto debugFont = GetUserResources()->GetDebugFont();
	debugFont->AddString(L"ModelSampleScene", SimpleMath::Vector2(0.0f, debugFont->GetFontHeight()));

}

void ModelSampleScene::Finalize()
{
}

void ModelSampleScene::CreateDeviceDependentResources()
{
}

void ModelSampleScene::CreateWindowSizeDependentResources()
{
}

void ModelSampleScene::OnDeviceLost()
{
	Finalize();
}
