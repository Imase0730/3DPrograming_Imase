#pragma once

#include "ImaseLib/SceneManager.h"
#include "UserResources.h"
#include "ImaseLib/GridFloor.h"
#include "ImaseLib/DebugCamera.h"
#include "ImaseLib/DisplayCollision.h"
#include "Object.h"

class ModelSampleScene : public Imase::Scene<UserResources>
{
public:
	
	// �R���X�g���N�^
	ModelSampleScene();

	// ������
	void Initialize() override;

	// �X�V
	void Update(float elapsedTime) override;

	// �`��
	void Render() override;

	// �I������
	void Finalize() override;

	// �f�o�C�X�Ɉˑ����郊�\�[�X���쐬����֐�
	void CreateDeviceDependentResources() override;

	// �E�C���h�E�T�C�Y�Ɉˑ����郊�\�[�X���쐬����֐�
	void CreateWindowSizeDependentResources() override;

	// �f�o�C�X���X�g�������ɌĂяo�����֐�
	void OnDeviceLost() override;

private:

	// �f�o�b�O�J�����ւ̃|�C���^
	std::unique_ptr<Imase::DebugCamera> m_debugCamera;

	// �r���[�s��
	DirectX::SimpleMath::Matrix m_view;

	// �ˉe�s��
	DirectX::SimpleMath::Matrix m_proj;

	// �O���b�h�̏��ւ̃|�C���^
	std::unique_ptr<Imase::GridFloor> m_gridFloor;

	// ��s�@���f���ւ̃|�C���^
	std::unique_ptr<DirectX::Model> m_planeModel;

private:

	// �x�[�V�b�N�G�t�F�N�g�ւ̃|�C���^
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;

	// �v���~�e�B�u�o�b�`�ւ̃|�C���^
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;

	// ���̓��C�A�E�g�ւ̃|�C���^
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

private:

	// ��s�@�̈ʒu
	DirectX::SimpleMath::Vector3 m_planePos;

	// ��s�@�̎p����\����]�N�H�[�^�j�I��
	DirectX::SimpleMath::Quaternion m_rotate;

private:

	// �Փ˔���̕\���I�u�W�F�N�g�ւ̃|�C���^
	std::unique_ptr<Imase::DisplayCollision> m_displayCollision;

	// �Փ˔���p�I�u�W�F�N�g
	CollisionTest::Object m_object[2];

	// �I�𒆂̃I�u�W�F�N�g�̔ԍ�
	int m_selectNo;

	// ���Ƌ��̏Փ˔���
	bool HitCheck_Sphere2Sphere(
		const DirectX::BoundingSphere& sphere1,
		const DirectX::BoundingSphere& sphere2
	);
};

