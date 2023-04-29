#pragma once

#include "../ImaseLib/SceneManager.h"
#include "../UserResources.h"
#include "../ImaseLib/TaskManager.h"
#include "../ImaseLib/ResourceManager.h"
#include "../Task/Number.h"

class SpriteSampleScene : public Imase::Scene<UserResources>
{
public:
	
	// �R���X�g���N�^
	SpriteSampleScene();

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

	// �X�v���C�g�o�b�`�ւ̃|�C���^
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

	// �L�̊G�̃e�N�X�`���n���h��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_catSRV;

	// ���{�b�g�̊G�̃e�N�X�`���n���h��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_robotSRV;

	// �J�[�h�̊G�̃e�N�X�`���n���h��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cardSRV[3];

	// �����̊G�̃e�N�X�`���n���h��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_numberSRV;

	// �΂ˌW��
	static const float K;

	// ���C�W��
	static const float FRICTION;

	// ���{�b�g�̊G�̃X�P�[��
	float m_robotScale;

	// ���{�b�g�̊G�̃X�P�[���̑��x
	float m_robotVelocity;

	// �^�X�N�}�l�[�W���[
	Imase::TaskManager m_taskManager;

	// ���\�[�X�}�l�[�W���[
	Imase::ResourceManager m_resourceManager;

	// ������\������^�X�N�ւ̃|�C���^
	Number* m_number;

};

