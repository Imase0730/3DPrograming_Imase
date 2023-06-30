#include "pch.h"
#include "GameCamera.h"

using namespace DirectX;

// コンストラクタ
GameCamera::GameCamera()
	: m_type(Type::Type_A)
	, m_angle(0.0f)
	, m_pPlayerPos{}
	, m_pPlayerRotate{}
{
}

// 更新関数
void GameCamera::Update(float elapsedTime)
{
	switch (m_type)
	{
	case Type::Type_A:	// プレイヤーの後ろから追いかけるカメラ
		{
			SimpleMath::Vector3 targetPos =
				*m_pPlayerPos + SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
			SimpleMath::Vector3 eyePos(0.0f, 2.0f, -3.0f);
			eyePos = SimpleMath::Vector3::Transform(eyePos, *m_pPlayerRotate);
			SetPositionTarget(targetPos + eyePos, targetPos);
		}
		break;
	case Type::Type_B:	// プレイヤーの周りを回るカメラ
		{
			m_angle += XMConvertToRadians(30.0f) * elapsedTime;
			SimpleMath::Vector3 targetPos =
				*m_pPlayerPos + SimpleMath::Vector3(0.0f, 0.3f, 0.0f);
			SimpleMath::Vector3 eyePos(0.0f, 1.0f, 2.0f);
			eyePos = SimpleMath::Vector3::Transform(
				eyePos, SimpleMath::Matrix::CreateRotationY(m_angle));
			SetPositionTarget(targetPos + eyePos, targetPos);
		}
	break;
	default:
		break;
	}
}


