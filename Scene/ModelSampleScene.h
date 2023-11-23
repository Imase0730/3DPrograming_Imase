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
#include "ImaseLib/DepthStencil.h"

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

	// �����_�[�e�N�X�`��(�V�[���S��)
	std::unique_ptr<DX::RenderTexture> m_offscreenRT;

	// �����_�[�e�N�X�`��(�u���[���p)
	std::unique_ptr<DX::RenderTexture> m_blur1RT;
	std::unique_ptr<DX::RenderTexture> m_blur2RT;

	// �X�v���C�g�o�b�`
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

	// �x�[�V�b�N�|�X�g�v���Z�X
	std::unique_ptr<DirectX::BasicPostProcess> m_basicPostProcess;

	// �f���A���|�X�g�v���Z�X
	std::unique_ptr<DirectX::DualPostProcess> m_dualPostProcess;

private:
	
	// �V���h�E�}�b�v�̃T�C�Y
	static const int SHADOWMAP_SIZE = 512;

	// �V���h�E�}�b�v�p�i�����_�[�e�N�X�`���j
	std::unique_ptr<DX::RenderTexture> m_shadowMapRT;

	// �V���h�E�}�b�v�p�i�f�v�X�X�e���V���j
	std::unique_ptr<Imase::DepthStencil> m_shadowMapDS;

	// ���_�V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VS_Depth;

	// �s�N�Z���V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PS_Depth;

	// ���C�g�̈ʒu
	DirectX::SimpleMath::Vector3 m_lightPosition;

	// ���C�g�̉�]
	DirectX::SimpleMath::Quaternion m_lightRotate;

private:

	// �萔�o�b�t�@�̍\����
	struct ConstantBuffer
	{
		DirectX::XMMATRIX lightViewProj;	// ���C�g�̓��e��Ԃ֍��W�ϊ�����s��
		DirectX::XMVECTOR lightPosition;	// ���C�g�̈ʒu
		DirectX::XMVECTOR lightAmbient;		// ���C�g�̊���
	};

	// �萔�o�b�t�@�ւ̃|�C���^
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;

	// ���_�V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VS;

	// �s�N�Z���V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PS;

};

