//--------------------------------------------------------------------------------------
// File: ModelCollision.h
//
// モデル用衝突判定クラス
//
// Date: 2023.6.10
// Author: Hideyasu Imase
//
//--------------------------------------------------------------------------------------
#pragma once

#include "ImaseLib/DisplayCollision.h"

namespace Imase
{
	class ModelCollisionSphere;
	class ModelCollisionBox;
	class ModelCollisionOrientedBox;

	// 衝突判定の基底クラス
	class ModelCollision
	{
	public:

		// コリジョンのタイプ
		enum class CollisionType
		{
			None,
			Sphere,
			AABB,
			OBB,
		};

	private:

		// コリジョンタイプ
		CollisionType m_type;

	public:

		// コンストラクタ
		ModelCollision(CollisionType type);

		// デストラクタ
		virtual ~ModelCollision() {}

		// 衝突判定を行う関数
		virtual bool Intersects(ModelCollision* collision) = 0;

		// 内包判定を行う関数
		virtual bool Contains(ModelCollision* collision) = 0;

		// コリジョンタイプの取得関数
		CollisionType GetType() { return m_type; }

		// コリジョン情報の更新
		virtual void UpdateBoundingInfo(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Quaternion rotate) = 0;

		// 衝突判定の表示に登録する関数
		virtual void AddDisplayCollision(Imase::DisplayCollision* displayCollision) = 0;

	protected:

		// 衝突判定（複数メッシュに対応）
		template <class T, class U>
		bool Intersects(T a, U b)
		{
			for (auto& data_a : a)
			{
				for (auto& data_b : b)
				{
					if (data_a.Intersects(data_b))
					{
						return true;
					}
				}
			}
			return false;
		}

		// 衝突判定
		// T この関数を呼び出すクラスの型（CollisionSphereなど）
		// U クラスのGetBoundingInfo関数が返す型（DirectX::BoundingSphereなど）
		template <class T, class U>
		bool Intersects(T* a, ModelCollision* b)
		{
			if (b == nullptr) return false;

			switch (b->GetType())
			{
			case CollisionType::Sphere:
			{
				auto p = dynamic_cast<ModelCollisionSphere*>(b);
				assert(p);
				return ModelCollision::Intersects<const std::vector<U>&, const std::vector<DirectX::BoundingSphere>&>
					(a->GetBoundingInfo(), p->GetBoundingInfo());
			}
			case CollisionType::AABB:
			{
				auto p = dynamic_cast<ModelCollisionBox*>(b);
				assert(p);
				return ModelCollision::Intersects<const std::vector<U>&, const std::vector<DirectX::BoundingBox>&>
					(a->GetBoundingInfo(), p->GetBoundingInfo());
			}
			case CollisionType::OBB:
			{
				auto p = dynamic_cast<ModelCollisionOrientedBox*>(b);
				assert(p);
				return ModelCollision::Intersects<const std::vector<U>&, const std::vector<DirectX::BoundingOrientedBox>&>
					(a->GetBoundingInfo(), p->GetBoundingInfo());
			}
			default:
				break;
			}
			return false;
		}

		// 内包判定
		template <class T, class U>
		bool Contains(T a, U b)
		{
			for (auto& data_a : a)
			{
				for (auto& data_b : b)
				{
					if (data_a.Contains(data_b) != DirectX::CONTAINS)
					{
						return false;
					}
				}
			}
			return true;
		}

		// 内包判定
		// T この関数を呼び出すクラスの型（CollisionSphereなど）
		// U クラスのGetBoundingInfo関数が返す型（DirectX::BoundingSphereなど）
		template <class T, class U>
		bool Contains(T* a, ModelCollision* b)
		{
			if (b == nullptr) return false;

			switch (b->GetType())
			{
			case CollisionType::Sphere:
			{
				auto p = dynamic_cast<ModelCollisionSphere*>(b);
				assert(p);
				return ModelCollision::Contains<const std::vector<U>&, const std::vector<DirectX::BoundingSphere>&>
					(a->GetBoundingInfo(), p->GetBoundingInfo());
			}
			case CollisionType::AABB:
			{
				auto p = dynamic_cast<ModelCollisionBox*>(b);
				assert(p);
				return ModelCollision::Contains<const std::vector<U>&, const std::vector<DirectX::BoundingBox>&>
					(a->GetBoundingInfo(), p->GetBoundingInfo());
			}
			case CollisionType::OBB:
			{
				auto p = dynamic_cast<ModelCollisionOrientedBox*>(b);
				assert(p);
				return ModelCollision::Contains<const std::vector<U>&, const std::vector<DirectX::BoundingOrientedBox>&>
					(a->GetBoundingInfo(), p->GetBoundingInfo());
			}
			default:
				break;
			}
			return false;
		}

	};

	// 衝突判定クラス（球）
	class ModelCollisionSphere : public ModelCollision
	{
	private:

		// 衝突判定情報
		std::vector<DirectX::BoundingSphere*> m_srcSpheres;

		// 衝突判定情報（ワールド空間）
		std::vector<DirectX::BoundingSphere> m_dstSpheres;

	public:

		// コンストラクタ
		ModelCollisionSphere(DirectX::Model* pModel);

		// 衝突判定情報の更新
		void UpdateBoundingInfo(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Quaternion rotate) override;

		// 衝突判定を行う関数
		bool Intersects(ModelCollision* collision) override;

		// 内包判定を行う関数
		bool Contains(ModelCollision* collision) override;

		// 衝突判定情報取得関数
		const std::vector<DirectX::BoundingSphere>& GetBoundingInfo() { return m_dstSpheres; }

		// 衝突判定の表示に登録する関数
		void AddDisplayCollision(Imase::DisplayCollision* displayCollision);

	};

	// 衝突判定クラス（AABB）
	class ModelCollisionBox : public ModelCollision
	{
	private:

		// 衝突判定情報
		std::vector<DirectX::BoundingBox*> m_srcBoxes;

		// 衝突判定情報（ワールド空間）
		std::vector<DirectX::BoundingBox> m_dstBoxes;

	public:

		// コンストラクタ
		ModelCollisionBox(DirectX::Model* pModel);

		// 衝突判定情報の更新
		void UpdateBoundingInfo(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Quaternion rotate) override;

		// 衝突判定を行う関数
		bool Intersects(ModelCollision* collision) override;

		// 内包判定を行う関数
		bool Contains(ModelCollision* collision) override;

		// 衝突判定情報取得関数
		const std::vector<DirectX::BoundingBox>& GetBoundingInfo() { return m_dstBoxes; }

		// 衝突判定の表示に登録する関数
		void AddDisplayCollision(Imase::DisplayCollision* displayCollision);

	};

	// 衝突判定クラス（OBB）
	class ModelCollisionOrientedBox : public ModelCollision
	{
	private:

		// 衝突判定情報
		std::vector<DirectX::BoundingBox*> m_srcBoxes;

		// 衝突判定情報（ワールド空間）
		std::vector<DirectX::BoundingOrientedBox> m_dstOrientedBoxes;

	public:

		// コンストラクタ
		ModelCollisionOrientedBox(DirectX::Model* pModel);

		// 衝突判定情報の更新
		void UpdateBoundingInfo(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Quaternion rotate) override;

		// 衝突判定を行う関数
		bool Intersects(ModelCollision* collision) override;

		// 内包判定を行う関数
		bool Contains(ModelCollision* collision) override;

		// 衝突判定情報取得関数
		const std::vector<DirectX::BoundingOrientedBox>& GetBoundingInfo() { return m_dstOrientedBoxes; }

		// 衝突判定の表示に登録する関数
		void AddDisplayCollision(Imase::DisplayCollision* displayCollision);

	};

	// 衝突判定生成クラス
	class ModelCollisionFactory
	{
	public:

		static std::unique_ptr<ModelCollision> CreateCollision(ModelCollision::CollisionType type, DirectX::Model* pModel)
		{
			std::unique_ptr<ModelCollision> collision;

			switch (type)
			{
			case ModelCollision::CollisionType::Sphere:
				collision = std::make_unique<ModelCollisionSphere>(pModel);
				break;
			case ModelCollision::CollisionType::AABB:
				collision = std::make_unique<ModelCollisionBox>(pModel);
				break;
			case ModelCollision::CollisionType::OBB:
				collision = std::make_unique<ModelCollisionOrientedBox>(pModel);
				break;
			default:
				break;
			}

			return collision;
		}
	};
}

