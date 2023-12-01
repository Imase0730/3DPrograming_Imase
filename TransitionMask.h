//--------------------------------------------------------------------------------------
// File: TransitionMask.h
//
// ��ʐ؂�ւ��p�̃}�X�N�\���N���X
//
// Date: 2023.9.10
// Author: Hideyasu Imase
//--------------------------------------------------------------------------------------
#pragma once

class TransitionMask
{
private:

	// ����(0�`1)
	float m_rate;

	// �I�[�v�����̓N���[�Y����܂ł̎���
	float m_interval;

	// �I�[�v���t���O�itrue�̏ꍇ�I�[�v���j
	bool m_open;

	// �X�v���C�g�o�b�`
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

public:

	// �R���X�g���N�^
	TransitionMask(
		ID3D11Device* device,
		ID3D11DeviceContext* context,
		float interval
	);

	// �X�V����
	void Update(float elapsedTime);

	// �`�揈��
	void Draw(
		ID3D11DeviceContext* context,
		DirectX::CommonStates* states,
		ID3D11ShaderResourceView* texture,
		const RECT& rect
	);

	// �I�[�v������֐�
	void Open();

	// �N���[�Y����֐�
	void Close();

	// �I�[�v�������`�F�b�N����֐�
	bool IsOpen();

	// �N���[�Y�����`�F�b�N����֐�
	bool IsClose();

	// �I�[�v�����Ԃ��֐�(0�`1)
	float GetOpenRate() { return m_rate; }

	// �I�[�v���A�N���[�Y�܂ł̎��Ԃ�ݒ肷��֐�
	void SetInterval(float interval) { m_interval = interval; }

};
