#pragma once

#include "ImaseLib/SceneManager.h"
#include "UserResources.h"
#include "ImaseLib/GridFloor.h"
#include "ImaseLib/DebugCamera.h"
#include "ImaseLib/DisplayCollision.h"
#include "Object.h"
#include "ImaseLib/ModelCollision.h"
#include "ImaseLib/ObjCollision.h"
#include "GameCamera.h"

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

	// �ԃ��f���ւ̃|�C���^
	std::unique_ptr<DirectX::Model> m_carModel;

	// �X�J�C�h�[�����f���ւ̃|�C���^
	std::unique_ptr<DirectX::Model> m_skydomeModel;

	// ��ԃ��f���ւ̃|�C���^
	std::unique_ptr<DirectX::Model> m_tankModel;

private:

	// �Փ˔���̕\���I�u�W�F�N�g�ւ̃|�C���^
	std::unique_ptr<Imase::DisplayCollision> m_displayCollision;

	// �Փ˔���p�I�u�W�F�N�g
	CollisionTest::Object m_object[2];

	// �I�𒆂̃I�u�W�F�N�g�̔ԍ�
	int m_selectNo;

	// �Փ˔���p���b�V���ւ̃|�C���^
	std::unique_ptr<Imase::ObjCollision> m_objCollision;

	// ����
	DirectX::SimpleMath::Vector3 m_line[2];


private:

	// ��Ԃ̈ʒu
	DirectX::SimpleMath::Vector3 m_tankPosition;

	// ��Ԃ̉�]
	DirectX::SimpleMath::Quaternion m_tankRotate;

	// �J����
	GameCamera m_camera;

};

