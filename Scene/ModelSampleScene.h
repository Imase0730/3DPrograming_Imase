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
#include "RenderTexture.h"

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

private:

	// �����f���ւ̃|�C���^
	std::unique_ptr<DirectX::Model> m_floorModel;

private:

	// �g�[���X�̃��f��
	std::unique_ptr<DirectX::Model> m_torusModel;

	// �s�N�Z���V�F�[�_�[�i�g�[���X�p�j
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PS_Torus;

private:

	// �萔�o�b�t�@�̍\����
	struct ConstantBuffer
	{
		float att0;		// ��茸���W��
		float att1;		// ���`�����W��
		float att2;		// �������W��
		float padding1;
		DirectX::XMVECTOR lightPosition;	// ���C�g�̈ʒu
	};

	// �����W��
	float m_att[3];

	// ���C�g�̈ʒu
	DirectX::SimpleMath::Vector3 m_lightPosition;

	// �萔�o�b�t�@�ւ̃|�C���^
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;

	// �����W���f�o�b�O�\���̃J�[�\���ʒu
	int m_cursor;

private:

	// �V�[���̕`��
	void DrawScene(const D3D11_VIEWPORT& vp, const DirectX::SimpleMath::Matrix& view);

private:

	// �����_�[�e�N�X�`��(�V�[���S��)
	std::unique_ptr<DX::RenderTexture> m_sceneRT;

	// �X�v���C�g�o�b�`
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

	// �x�[�V�b�N�|�X�g�v���Z�X
	std::unique_ptr<DirectX::BasicPostProcess> m_basicPostProcess;

};

