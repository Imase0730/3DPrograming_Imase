//--------------------------------------------------------------------------------------
// File: ModelCollision.cpp
//
// モデル用衝突判定クラス
//
// Date: 2023.6.13
// Author: S.Takaki
//
//--------------------------------------------------------------------------------------
#include "pch.h"
#include "ModelCollision.h"

using namespace DirectX;
using namespace Imase;

// 衝突判定形状クラス
template <typename T>
class CollisionGeometry : public ICollisionGeometry
{
private:
	// メッシュ
	const std::weak_ptr<DirectX::ModelMesh> m_mesh;

	// 衝突判定形状（ワールド空間）
	T m_geometry;

public:
	// コンストラクタ
	CollisionGeometry(const std::shared_ptr<DirectX::ModelMesh>& mesh);

	// デストラクタ
	~CollisionGeometry() = default;

	// 更新
	void Update(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Quaternion& rotate) override;

	// 衝突判定の表示に登録する関数
	void AddDisplayCollision(Imase::DisplayCollision* displayCollision) const override;

	// 衝突判定を行う関数(ディスパッチ用)
	bool Intersects(const std::unique_ptr<ICollisionGeometry>& geometry) const override;

	// 衝突判定を行う関数(実際の判定用)
	bool Intersects(const DirectX::BoundingSphere& geometry) const override;
	bool Intersects(const DirectX::BoundingBox& geometry) const override;
	bool Intersects(const DirectX::BoundingOrientedBox& geometry) const override;

	// モデルから形状コレクションの生成関数
	static ICollisionGeometry::Collection CreateCollectionFromModel(const DirectX::Model* pModel);
};

// エイリアス宣言
using CollisionSphere      = CollisionGeometry<DirectX::BoundingSphere>;
using CollisionBox         = CollisionGeometry<DirectX::BoundingBox>;
using CollisionOrientedBox = CollisionGeometry<DirectX::BoundingOrientedBox>;

// ------------------------------------------------------------------------- //

// コンストラクタ
template<typename T>
CollisionGeometry<T>::CollisionGeometry(const std::shared_ptr<DirectX::ModelMesh>& mesh)
	: m_mesh{ mesh }
	, m_geometry{}
{
}

// 更新
template<typename T>
void CollisionGeometry<T>::Update(
	const DirectX::SimpleMath::Vector3& position,
	const DirectX::SimpleMath::Quaternion& rotate
)
{
}

// 衝突判定の表示に登録する関数
template <typename T>
void CollisionGeometry<T>::AddDisplayCollision(Imase::DisplayCollision* displayCollision) const
{
	displayCollision->AddBoundingVolume(m_geometry);
}

// 衝突判定を行う関数(ディスパッチ用)
template <typename T>
bool CollisionGeometry<T>::Intersects(const std::unique_ptr<ICollisionGeometry>& geometry) const
{
	return geometry->Intersects(this->m_geometry);
};

// 衝突判定を行う関数(対球)
template <typename T>
bool CollisionGeometry<T>::Intersects(const DirectX::BoundingSphere& geometry) const
{
	return m_geometry.Intersects(geometry);
}

// 衝突判定を行う関数(対AABB)
template <typename T>
bool CollisionGeometry<T>::Intersects(const DirectX::BoundingBox& geometry) const
{
	return m_geometry.Intersects(geometry);
}

// 衝突判定を行う関数(対OBB)
template <typename T>
bool CollisionGeometry<T>::Intersects(const DirectX::BoundingOrientedBox& geometry) const
{
	return m_geometry.Intersects(geometry);
}

// モデルから衝突判定コレクションの生成関数
template <typename T>
ICollisionGeometry::Collection CollisionGeometry<T>::CreateCollectionFromModel(const DirectX::Model* pModel)
{
	// コリジョン情報をモデルデータから取得
	Collection geometries;
	for (auto& mesh : pModel->meshes)
	{
		geometries.push_back(std::make_unique<CollisionGeometry<T>>(mesh));
	}

	return std::move(geometries);
}

// ------------------------------------------------------------------------- //

// 衝突判定形状（球）用コンストラクタ
CollisionSphere::CollisionGeometry(const std::shared_ptr<DirectX::ModelMesh>& mesh)
	: m_mesh{ mesh }
	, m_geometry{ mesh->boundingSphere }
{
}

// 衝突判定形状（球）用更新関数
void CollisionSphere::Update(
	const DirectX::SimpleMath::Vector3& position,
	const DirectX::SimpleMath::Quaternion& rotate
)
{
	if (std::shared_ptr<DirectX::ModelMesh> mesh = m_mesh.lock())
	{
		m_geometry.Center = DirectX::SimpleMath::Vector3::Transform(mesh->boundingSphere.Center, rotate) + position;
		m_geometry.Radius = mesh->boundingSphere.Radius;
	}
}

// ------------------------------------------------------------------------- //

// 衝突判定形状（AABB）用コンストラクタ
CollisionBox::CollisionGeometry(const std::shared_ptr<DirectX::ModelMesh>& mesh)
	: m_mesh{ mesh }
	, m_geometry{ mesh->boundingBox }
{
}

// 衝突判定形状（AABB）用更新関数
void CollisionBox::Update(
	const DirectX::SimpleMath::Vector3& position,
	const DirectX::SimpleMath::Quaternion& rotate
)
{
	if (std::shared_ptr<DirectX::ModelMesh> mesh = m_mesh.lock())
	{
		m_geometry.Center  = DirectX::SimpleMath::Vector3::Transform(mesh->boundingBox.Center, rotate) + position;
		m_geometry.Extents = mesh->boundingBox.Extents;
	}
}

// ------------------------------------------------------------------------- //

// 衝突判定形状（OBB）用コンストラクタ
CollisionOrientedBox::CollisionGeometry(const std::shared_ptr<DirectX::ModelMesh>& mesh)
	: m_mesh{ mesh }
	, m_geometry{ }
{
	DirectX::BoundingOrientedBox::CreateFromBoundingBox(m_geometry, mesh->boundingBox);
}


// 衝突判定形状（AABB）用更新関数
void CollisionOrientedBox::Update(
	const DirectX::SimpleMath::Vector3& position,
	const DirectX::SimpleMath::Quaternion& rotate
)
{
	if (std::shared_ptr<DirectX::ModelMesh> mesh = m_mesh.lock())
	{
		m_geometry.Center      = DirectX::SimpleMath::Vector3::Transform(mesh->boundingBox.Center, rotate) + position;
		m_geometry.Extents     = mesh->boundingBox.Extents;
		m_geometry.Orientation = rotate;
	}
}

// ------------------------------------------------------------------------- //

// コンストラクタ
ModelCollision::ModelCollision(
	CollisionType type,
	const DirectX::Model* pModel,
	GeometryFactory geometryFactory
)
	: m_type(type)
	, m_geometries{ geometryFactory(pModel) }
{
}

ModelCollision::ModelCollision(
	CollisionType type,
	ICollisionGeometry::Collection&& geometries
)
	: m_type(type)
	, m_geometries{ std::move(geometries) }
{
}

// 衝突判定を行う関数
bool ModelCollision::Intersects(const ModelCollision* collision) const
{
	for (auto& data_a : m_geometries)
	{
		for (auto& data_b : collision->m_geometries)
		{
			if (data_a->Intersects(data_b))
			{
				return true;
			}
		}
	}
	return false;
}

// コリジョン情報の更新
void ModelCollision::UpdateBoundingInfo(
	const DirectX::SimpleMath::Vector3& position,
	const DirectX::SimpleMath::Quaternion& rotate
)
{
	for (auto& geometry : m_geometries) {
		geometry->Update(position, rotate);
	}
}

// 衝突判定の表示に登録する関数
void ModelCollision::AddDisplayCollision(Imase::DisplayCollision* displayCollision) const
{
	for (auto& geometry : m_geometries) {
		geometry->AddDisplayCollision(displayCollision);
	}
}

// ------------------------------------------------------------------------- //

// 衝突判定生成
std::unique_ptr<ModelCollision> CollisionFactory::CreateCollision(const DirectX::Model* pModel, ModelCollision::CollisionType type)
{
	std::unique_ptr<ModelCollision> collision;

	switch (type)
	{
	case ModelCollision::CollisionType::Sphere:
		collision = std::make_unique<ModelCollision>(type, CollisionSphere::CreateCollectionFromModel(pModel));
		break;
	case ModelCollision::CollisionType::AABB:
		collision = std::make_unique<ModelCollision>(type, CollisionBox::CreateCollectionFromModel(pModel));
		break;
	case ModelCollision::CollisionType::OBB:
		collision = std::make_unique<ModelCollision>(type, CollisionOrientedBox::CreateCollectionFromModel(pModel));
		break;
	default:
		break;
	}

	return collision;
}
