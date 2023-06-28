#include "pch.h"
#include "Camera.h"

Camera::Camera()
{
}

void Camera::SetPositionTarget(const DirectX::SimpleMath::Vector3& eye, const DirectX::SimpleMath::Vector3& target)
{
	m_eyePt = eye;
	m_targetPt = target;
}
