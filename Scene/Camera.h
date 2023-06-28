#pragma once


class Camera
{
private:

	// 視点の位置
	DirectX::SimpleMath::Vector3 m_eyePt;

	// 注目点の位置
	DirectX::SimpleMath::Vector3 m_targetPt;

public:

	// コンストラクタ
	Camera();

	// デストラクタ
	virtual ~Camera() {}

	// カメラの位置とターゲットを指定する関数
	void SetPositionTarget(const DirectX::SimpleMath::Vector3& eye, const DirectX::SimpleMath::Vector3& target);

	// 視点の位置を取得する関数
	const DirectX::SimpleMath::Vector3& GetEyePosition() { return m_eyePt; }

	// 注目点の位置を取得する関数
	const DirectX::SimpleMath::Vector3& GetTargetPosition() { return m_targetPt; }

};
