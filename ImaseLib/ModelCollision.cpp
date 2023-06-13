//--------------------------------------------------------------------------------------
// File: ModelCollision.cpp
//
// ���f���p�Փ˔���N���X
//
// Date: 2023.6.13
// Author: S.Takaki
//
//--------------------------------------------------------------------------------------
#include "pch.h"
#include "ModelCollision.h"

using namespace DirectX;
using namespace Imase;

// �Փ˔���`��N���X
template <typename T>
class CollisionGeometry : public ICollisionGeometry
{
private:
	// ���b�V��
	const std::weak_ptr<DirectX::ModelMesh> m_mesh;

	// �Փ˔���`��i���[���h��ԁj
	T m_geometry;

public:
	// �R���X�g���N�^
	CollisionGeometry(const std::shared_ptr<DirectX::ModelMesh>& mesh);

	// �f�X�g���N�^
	~CollisionGeometry() = default;

	// �X�V
	void Update(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Quaternion& rotate) override;

	// �Փ˔���̕\���ɓo�^����֐�
	void AddDisplayCollision(Imase::DisplayCollision* displayCollision) const override;

	// �Փ˔�����s���֐�(�f�B�X�p�b�`�p)
	bool Intersects(const std::unique_ptr<ICollisionGeometry>& geometry) const override;

	// �Փ˔�����s���֐�(���ۂ̔���p)
	bool Intersects(const DirectX::BoundingSphere& geometry) const override;
	bool Intersects(const DirectX::BoundingBox& geometry) const override;
	bool Intersects(const DirectX::BoundingOrientedBox& geometry) const override;

	// ���f������`��R���N�V�����̐����֐�
	static ICollisionGeometry::Collection CreateCollectionFromModel(const DirectX::Model* pModel);
};

// �G�C���A�X�錾
using CollisionSphere      = CollisionGeometry<DirectX::BoundingSphere>;
using CollisionBox         = CollisionGeometry<DirectX::BoundingBox>;
using CollisionOrientedBox = CollisionGeometry<DirectX::BoundingOrientedBox>;

// ------------------------------------------------------------------------- //

// �R���X�g���N�^
template<typename T>
CollisionGeometry<T>::CollisionGeometry(const std::shared_ptr<DirectX::ModelMesh>& mesh)
	: m_mesh{ mesh }
	, m_geometry{}
{
}

// �X�V
template<typename T>
void CollisionGeometry<T>::Update(
	const DirectX::SimpleMath::Vector3& position,
	const DirectX::SimpleMath::Quaternion& rotate
)
{
}

// �Փ˔���̕\���ɓo�^����֐�
template <typename T>
void CollisionGeometry<T>::AddDisplayCollision(Imase::DisplayCollision* displayCollision) const
{
	displayCollision->AddBoundingVolume(m_geometry);
}

// �Փ˔�����s���֐�(�f�B�X�p�b�`�p)
template <typename T>
bool CollisionGeometry<T>::Intersects(const std::unique_ptr<ICollisionGeometry>& geometry) const
{
	return geometry->Intersects(this->m_geometry);
};

// �Փ˔�����s���֐�(�΋�)
template <typename T>
bool CollisionGeometry<T>::Intersects(const DirectX::BoundingSphere& geometry) const
{
	return m_geometry.Intersects(geometry);
}

// �Փ˔�����s���֐�(��AABB)
template <typename T>
bool CollisionGeometry<T>::Intersects(const DirectX::BoundingBox& geometry) const
{
	return m_geometry.Intersects(geometry);
}

// �Փ˔�����s���֐�(��OBB)
template <typename T>
bool CollisionGeometry<T>::Intersects(const DirectX::BoundingOrientedBox& geometry) const
{
	return m_geometry.Intersects(geometry);
}

// ���f������Փ˔���R���N�V�����̐����֐�
template <typename T>
ICollisionGeometry::Collection CollisionGeometry<T>::CreateCollectionFromModel(const DirectX::Model* pModel)
{
	// �R���W�����������f���f�[�^����擾
	Collection geometries;
	for (auto& mesh : pModel->meshes)
	{
		geometries.push_back(std::make_unique<CollisionGeometry<T>>(mesh));
	}

	return std::move(geometries);
}

// ------------------------------------------------------------------------- //

// �Փ˔���`��i���j�p�R���X�g���N�^
CollisionSphere::CollisionGeometry(const std::shared_ptr<DirectX::ModelMesh>& mesh)
	: m_mesh{ mesh }
	, m_geometry{ mesh->boundingSphere }
{
}

// �Փ˔���`��i���j�p�X�V�֐�
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

// �Փ˔���`��iAABB�j�p�R���X�g���N�^
CollisionBox::CollisionGeometry(const std::shared_ptr<DirectX::ModelMesh>& mesh)
	: m_mesh{ mesh }
	, m_geometry{ mesh->boundingBox }
{
}

// �Փ˔���`��iAABB�j�p�X�V�֐�
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

// �Փ˔���`��iOBB�j�p�R���X�g���N�^
CollisionOrientedBox::CollisionGeometry(const std::shared_ptr<DirectX::ModelMesh>& mesh)
	: m_mesh{ mesh }
	, m_geometry{ }
{
	DirectX::BoundingOrientedBox::CreateFromBoundingBox(m_geometry, mesh->boundingBox);
}


// �Փ˔���`��iAABB�j�p�X�V�֐�
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

// �R���X�g���N�^
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

// �Փ˔�����s���֐�
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

// �R���W�������̍X�V
void ModelCollision::UpdateBoundingInfo(
	const DirectX::SimpleMath::Vector3& position,
	const DirectX::SimpleMath::Quaternion& rotate
)
{
	for (auto& geometry : m_geometries) {
		geometry->Update(position, rotate);
	}
}

// �Փ˔���̕\���ɓo�^����֐�
void ModelCollision::AddDisplayCollision(Imase::DisplayCollision* displayCollision) const
{
	for (auto& geometry : m_geometries) {
		geometry->AddDisplayCollision(displayCollision);
	}
}

// ------------------------------------------------------------------------- //

// �Փ˔��萶��
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
