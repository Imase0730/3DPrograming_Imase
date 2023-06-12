//--------------------------------------------------------------------------------------
// File: Collision.cpp
//
// モデル用衝突判定クラス
//
// Date: 2023.6.10
// Author: Hideyasu Imase
//
//--------------------------------------------------------------------------------------
#include "pch.h"
#include "ModelCollision.h"

using namespace DirectX;
using namespace Imase;

// コンストラクタ
ModelCollision::ModelCollision(CollisionType type)
	: m_type(type)
{
}

// ------------------------------------------------------------------------------- //

// コンストラクタ
ModelCollisionSphere::ModelCollisionSphere(DirectX::Model* pModel)
	: ModelCollision(ModelCollision::CollisionType::Sphere)
{
	// コリジョン情報をモデルデータから取得
	for (auto& mesh : pModel->meshes) {
		m_srcSpheres.push_back(&mesh->boundingSphere);
	}
	// ワールド空間内のコリジョン情報を保存するワークを確保する
	m_dstSpheres.resize(pModel->meshes.size());
}

// 衝突判定情報の更新
void ModelCollisionSphere::UpdateBoundingInfo(
	DirectX::SimpleMath::Vector3 position,
	DirectX::SimpleMath::Quaternion rotate
)
{
	UNREFERENCED_PARAMETER(rotate);

	for (int i = 0; i < m_srcSpheres.size(); i++)
	{
		DirectX::BoundingSphere dst(*m_srcSpheres[i]);
		dst.Center = DirectX::SimpleMath::Vector3::Transform(m_srcSpheres[i]->Center, rotate) + position;
		m_dstSpheres[i] = dst;
	}
}

// 衝突判定を行う関数
bool ModelCollisionSphere::Intersects(ModelCollision* collision)
{
	return ModelCollision::Intersects<ModelCollisionSphere, DirectX::BoundingSphere>(this, collision);
}

// 内包判定を行う関数
bool ModelCollisionSphere::Contains(ModelCollision* collision)
{
	return ModelCollision::Contains<ModelCollisionSphere, DirectX::BoundingSphere>(this, collision);
}

// 衝突判定の表示に登録する関数
void ModelCollisionSphere::AddDisplayCollision(Imase::DisplayCollision* displayCollision)
{
	for (auto& data : GetBoundingInfo())
	{
		displayCollision->AddBoundingSphere(data);
	}
}

// ------------------------------------------------------------------------------- //

// コンストラクタ
ModelCollisionBox::ModelCollisionBox(DirectX::Model* pModel)
	: ModelCollision(ModelCollision::CollisionType::AABB)
{
	// コリジョン情報をモデルデータから取得
	for (auto& mesh : pModel->meshes) {
		m_srcBoxes.push_back(&mesh->boundingBox);
	}
	// ワールド空間内のコリジョン情報を保存するワークを確保する
	m_dstBoxes.resize(pModel->meshes.size());
}

// 衝突判定情報の更新
void ModelCollisionBox::UpdateBoundingInfo(
	DirectX::SimpleMath::Vector3 position,
	DirectX::SimpleMath::Quaternion rotate
)
{
	UNREFERENCED_PARAMETER(rotate);

	for (int i = 0; i < m_srcBoxes.size(); i++)
	{
		DirectX::BoundingBox dst(*m_srcBoxes[i]);
		dst.Center = DirectX::SimpleMath::Vector3::Transform(m_srcBoxes[i]->Center, rotate) + position;
		m_dstBoxes[i] = dst;
	}
}

// 衝突判定を行う関数
bool ModelCollisionBox::Intersects(ModelCollision* collision)
{
	return ModelCollision::Intersects<ModelCollisionBox, DirectX::BoundingBox>(this, collision);
}

// 内包判定を行う関数
bool ModelCollisionBox::Contains(ModelCollision* collision)
{
	return ModelCollision::Contains<ModelCollisionBox, DirectX::BoundingBox>(this, collision);
}

// 衝突判定の表示に登録する関数
void ModelCollisionBox::AddDisplayCollision(Imase::DisplayCollision* displayCollision)
{
	for (auto& data : GetBoundingInfo())
	{
		displayCollision->AddBoundingBox(data);
	}
}

// ------------------------------------------------------------------------------- //

// コンストラクタ
ModelCollisionOrientedBox::ModelCollisionOrientedBox(DirectX::Model* pModel)
	: ModelCollision(ModelCollision::CollisionType::OBB)
{
	// コリジョン情報をモデルデータから取得
	for (auto& mesh : pModel->meshes) {
		m_srcBoxes.push_back(&mesh->boundingBox);
	}
	// ワールド空間内のコリジョン情報を保存するワークを確保する
	m_dstOrientedBoxes.resize(pModel->meshes.size());
}

// 衝突判定情報の更新
void ModelCollisionOrientedBox::UpdateBoundingInfo(
	DirectX::SimpleMath::Vector3 position,
	DirectX::SimpleMath::Quaternion rotate
)
{
	for (int i = 0; i < m_srcBoxes.size(); i++)
	{
		DirectX::BoundingOrientedBox dst;
		DirectX::BoundingOrientedBox::CreateFromBoundingBox(dst, *m_srcBoxes[i]);
		dst.Center = DirectX::SimpleMath::Vector3::Transform(m_srcBoxes[i]->Center, rotate) + position;
		dst.Orientation = rotate;
		m_dstOrientedBoxes[i] = dst;
	}
}

// 衝突判定を行う関数
bool ModelCollisionOrientedBox::Intersects(ModelCollision* collision)
{
	return ModelCollision::Intersects<ModelCollisionOrientedBox, DirectX::BoundingOrientedBox>(this, collision);
}

// 内包判定を行う関数
bool ModelCollisionOrientedBox::Contains(ModelCollision* collision)
{
	return ModelCollision::Contains<ModelCollisionOrientedBox, DirectX::BoundingOrientedBox>(this, collision);
}

// 衝突判定の表示に登録する関数
void ModelCollisionOrientedBox::AddDisplayCollision(Imase::DisplayCollision* displayCollision)
{
	for (auto& data : GetBoundingInfo())
	{
		displayCollision->AddBoundingOrientedBox(data);
	}
}
