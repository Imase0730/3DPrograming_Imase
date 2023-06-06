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

		// 衝突判定（球）
		DirectX::BoundingSphere boundingSphere;

		// 衝突判定情報取得関数（球）
		DirectX::BoundingSphere GetBoundingSphere()
		{
			DirectX::BoundingSphere tmp(boundingSphere);
			tmp.Center.x += position.x;
			tmp.Center.y += position.y;
			tmp.Center.z += position.z;
			return tmp;
		}
	};
}
