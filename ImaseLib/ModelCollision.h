//--------------------------------------------------------------------------------------
// File: ModelCollision.h
//
// ���f���p�Փ˔���N���X
//
// Date: 2023.6.13
// Author: S.Takaki
//
//--------------------------------------------------------------------------------------
#pragma once

#include "ImaseLib/DisplayCollision.h"
#include <functional>

namespace Imase
{
	// �Փ˔���`��̃C���^�t�F�[�X
	class ICollisionGeometry
	{
	public:

		using Collection = std::vector<std::unique_ptr<ICollisionGeometry>>;

	public:

		// �f�X�g���N�^
		virtual ~ICollisionGeometry() = default;

		// �X�V
		virtual void Update(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Quaternion& rotate) = 0;

		// �Փ˔���̕\���ɓo�^����֐�
		virtual void AddDisplayCollision(Imase::DisplayCollision* displayCollision) const = 0;

		// �Փ˔�����s���֐�(�f�B�X�p�b�`�p)
		virtual bool Intersects(const std::unique_ptr<ICollisionGeometry>& geometry) const = 0;

		// �Փ˔�����s���֐�(���ۂ̔���p)
		virtual bool Intersects(const DirectX::BoundingSphere& geometry) const = 0;
		virtual bool Intersects(const DirectX::BoundingBox& geometry) const = 0;
		virtual bool Intersects(const DirectX::BoundingOrientedBox& geometry) const = 0;
	};

	// �Փ˔���̃N���X
	class ModelCollision
	{
		using GeometryFactory = std::function< ICollisionGeometry::Collection(const DirectX::Model* pModel)>;

	public:

		// �R���W�����̃^�C�v
		enum class CollisionType
		{
			None,
			Sphere,
			AABB,
			OBB,
		};

	public:

		// �R���W�����^�C�v
		CollisionType m_type;

		// �Փ˔�����
		ICollisionGeometry::Collection m_geometries;

	public:

		// �R���X�g���N�^
		ModelCollision(CollisionType type, const DirectX::Model* pModel, GeometryFactory geometryFactory);
		ModelCollision(CollisionType type, ICollisionGeometry::Collection&& geometries);

		// �f�X�g���N�^
		~ModelCollision() = default;

		// �Փ˔�����s���֐�
		bool Intersects(const ModelCollision* collision) const;

		// �R���W�����^�C�v�̎擾�֐�
		CollisionType GetType() { return m_type; }

		// �R���W�������̍X�V
		void UpdateBoundingInfo(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Quaternion& rotate);

		// �Փ˔���̕\���ɓo�^����֐�
		void AddDisplayCollision(Imase::DisplayCollision* displayCollision) const;
	};

	// �Փ˔��萶���N���X
	class ModelCollisionFactory
	{
	public:

		static std::unique_ptr<ModelCollision> CreateCollision(const DirectX::Model* pModel, ModelCollision::CollisionType type = ModelCollision::CollisionType::Sphere);
	};
}

