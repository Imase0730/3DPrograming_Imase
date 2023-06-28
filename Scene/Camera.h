#pragma once


class Camera
{
private:

	// ���_�̈ʒu
	DirectX::SimpleMath::Vector3 m_eyePt;

	// ���ړ_�̈ʒu
	DirectX::SimpleMath::Vector3 m_targetPt;

public:

	// �R���X�g���N�^
	Camera();

	// �f�X�g���N�^
	virtual ~Camera() {}

	// �J�����̈ʒu�ƃ^�[�Q�b�g���w�肷��֐�
	void SetPositionTarget(const DirectX::SimpleMath::Vector3& eye, const DirectX::SimpleMath::Vector3& target);

	// ���_�̈ʒu���擾����֐�
	const DirectX::SimpleMath::Vector3& GetEyePosition() { return m_eyePt; }

	// ���ړ_�̈ʒu���擾����֐�
	const DirectX::SimpleMath::Vector3& GetTargetPosition() { return m_targetPt; }

};
