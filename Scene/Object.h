#pragma once

namespace CollisionTest
{
	// 衝突判定用オブジェクトの構造体
	struct Object
	{
		// 位置
		DirectX::SimpleMath::Vector3 position;

		// 回転
		DirectX::SimpleMath::Quaternion rotate;

		// -------------------------------------------------------- //

		// 衝突判定（球）
		DirectX::BoundingSphere boundingSphere;

		// 衝突判定情報取得関数（球）
		DirectX::BoundingSphere GetBoundingSphere()
		{
			DirectX::BoundingSphere tmp(boundingSphere);
			tmp.Center = DirectX::SimpleMath::Vector3::Transform(boundingSphere.Center, rotate) + position;
			return tmp;
		}

		// -------------------------------------------------------- //

		// 衝突判定（AABB）
		DirectX::BoundingBox boundingBox;

		// 衝突判定情報取得関数（AABB）
		DirectX::BoundingBox GetBoundingBox()
		{
			DirectX::BoundingBox tmp(boundingBox);
			tmp.Center = DirectX::SimpleMath::Vector3::Transform(boundingBox.Center, rotate) + position;
			return tmp;
		}

		// -------------------------------------------------------- //

		// 衝突判定情報取得関数（OBB）
		DirectX::BoundingOrientedBox GetBoundingOrientedBox()
		{
			DirectX::BoundingOrientedBox tmp;
			DirectX::BoundingOrientedBox::CreateFromBoundingBox(tmp, boundingBox);
			tmp.Center = DirectX::SimpleMath::Vector3::Transform(boundingBox.Center, rotate) + position;
			tmp.Orientation = rotate;
			return tmp;
		}
	};
}
