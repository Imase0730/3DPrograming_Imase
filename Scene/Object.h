#pragma once

namespace CollisionTest
{
	// �Փ˔���p�I�u�W�F�N�g�̍\����
	struct Object
	{
		// �ʒu
		DirectX::SimpleMath::Vector3 position;

		// ��]
		DirectX::SimpleMath::Quaternion rotate;

		// �Փ˔���i���j
		DirectX::BoundingSphere boundingSphere;

		// �Փ˔�����擾�֐��i���j
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
