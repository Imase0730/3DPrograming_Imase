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
#include "ImaseLib/ModelPart.h"

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

	// �{�[�����f���ւ̃|�C���^
	std::unique_ptr<DirectX::Model> m_boneModel;

private:

	// ���{�b�g�̈ʒu
	DirectX::SimpleMath::Vector3 m_robotPosition;

	// ���{�b�g�̉�]
	DirectX::SimpleMath::Quaternion m_robotRotate;

	// ���{�b�g�̏㔼�g�̉�]
	DirectX::SimpleMath::Quaternion m_bodyRotate;

	// ���{�b�g�̍��r�̉�]
	DirectX::SimpleMath::Quaternion m_armRotate;

	// �J����
	GameCamera m_camera;

private:

	// ���{�b�g�̊e�p�[�c���f���ւ̃|�C���^
	std::unique_ptr<DirectX::Model> m_headModel;
	std::unique_ptr<DirectX::Model> m_bodyModel;
	std::unique_ptr<DirectX::Model> m_legModel;
	std::unique_ptr<DirectX::Model> m_armRModel;
	std::unique_ptr<DirectX::Model> m_armLModel;
	std::unique_ptr<DirectX::Model> m_missileModel;

	enum { ROOT, HEAD, BODY, LEG, ARM_R, ARM_L, MISSILE, PARTS_CNT };

	// ���{�b�g�̃p�[�c�ւ̃|�C���^
	std::unique_ptr<Imase::ModelPart> m_parts[PARTS_CNT];

	// �~�T�C�����˃t���O
	bool m_fireFlag;

	// �~�T�C���̈ʒu
	DirectX::SimpleMath::Vector3 m_missilePosition;

	// �~�T�C���̉�]
	DirectX::SimpleMath::Quaternion m_missileRotate;

	// �~�T�C���̈ړ�����
	float m_distance;

private:

	// �����f���ւ̃|�C���^
	std::unique_ptr<DirectX::Model> m_floorModel;

	// �e�̃e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shadowTexture;

	// �x�[�V�b�N�G�t�F�N�g
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;

	// �v���~�e�B�u�o�b�`
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>> m_primitiveBatch;

	// ���̓��C�A�E�g
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	// �e�̏������֐�
	void InitializeShadow(ID3D11Device* device, ID3D11DeviceContext* context);

	// �e�̏I������
	void ResetShadow();

	// �e�̕`��֐�
	void DrawShadow(
		ID3D11DeviceContext* context,
		DirectX::CommonStates* states,
		DirectX::SimpleMath::Vector3 position,
		float radius = 0.5f
	);

	// �[�x�X�e���V���X�e�[�g�i���j
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState_Floor;

	// �[�x�X�e���V���X�e�[�g�i�e�j
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState_Shadow;

};

