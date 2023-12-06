//--------------------------------------------------------------------------------------
// File: TransitionMask.cpp
//
// ��ʐ؂�ւ��p�̃}�X�N�\���N���X
//
// Date: 2023.9.10
// Author: Hideyasu Imase
//--------------------------------------------------------------------------------------
#include "pch.h"
#include "TransitionMask.h"
#include "ReadData.h"
#include "CommonSys.h"

using namespace DirectX;

// �R���X�g���N�^
TransitionMask::TransitionMask(
	ID3D11Device* device,
	ID3D11DeviceContext* context,
	float interval
)
	: m_interval(interval)
	, m_rate(0.0f)
	, m_open(true)
	, m_request(CreateMaskRequest::NONE)
{
	// �X�v���C�g�o�b�`�̍쐬
	m_spriteBatch = std::make_unique<SpriteBatch>(context);
}

// �X�V����
void TransitionMask::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	if (m_open)
	{
		// �I�[�v��
		m_rate -= FRAME_TIME / m_interval;
		if (m_rate < 0.0f) m_rate = 0.0f;
	}
	else
	{
		// �N���[�Y
		m_rate += FRAME_TIME / m_interval;
		if (m_rate > 1.0f) m_rate = 1.0f;
	}
}

// �`�揈��
void TransitionMask::Draw(
	ID3D11DeviceContext* context,
	DirectX::CommonStates* states,
	ID3D11ShaderResourceView* texture,
	const RECT& rect
)
{
	if (m_rate == 0.0f) return;

	m_spriteBatch->Begin(SpriteSortMode_Immediate, states->NonPremultiplied());

	m_spriteBatch->Draw(texture, rect, SimpleMath::Color(1.0f, 1.0f, 1.0f, m_rate));

	m_spriteBatch->End();
}

// �}�X�N���I�[�v������֐�
void TransitionMask::Open()
{
	m_open = true;
	m_rate = 1.0f;
}

// �}�X�N���N���[�Y����֐�
void TransitionMask::Close()
{
	m_open = false;
	m_rate = 0.0f;
}

//// �I�[�v�����Ă��邩�`�F�b�N����֐�
//bool TransitionMask::IsOpen() const
//{
//	if (m_open && m_rate == 0.0f) return true;
//	return false;
//}
//
//// �N���[�Y���Ă��邩�`�F�b�N����֐�
//bool TransitionMask::IsClose() const
//{
//	if (!m_open && m_rate == 1.0f) return true;
//	return false;
//}
