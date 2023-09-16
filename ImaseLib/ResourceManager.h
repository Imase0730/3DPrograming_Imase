//--------------------------------------------------------------------------------------
// File: ResourceManager.h
//
// ���\�[�X���Ǘ�����N���X
//
// Date: 2023.8.24
// Author: Hideyasu Imase
//--------------------------------------------------------------------------------------
#pragma once

#include <unordered_map>
#include <string>

namespace Imase
{
	class ResourceManager
	{
	private:

		// �t�@�C���̎��
		enum class Kind
		{
			UNKNOWN,
			DDS,
			CMO,
			WAV,
		};

		// �f�o�C�X
		ID3D11Device* m_device;

		// �E�F�[�u�o���N�ւ̃|�C���^
		DirectX::WaveBank* m_waveBank;

		// �e�N�X�`���L���b�V��
		using TextureCache = std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>;
		TextureCache m_textureCache;

		// ���f���L���b�V��
		using ModelCache = std::unordered_map<std::wstring, std::unique_ptr<DirectX::Model>>;
		ModelCache m_modelCache;

		// �T�E���h�L���b�V��
		using SoundCache = std::unordered_map<unsigned int, std::unique_ptr<DirectX::SoundEffectInstance>>;
		SoundCache m_soundCache;

		// �t�@�C���������ނ𔻒肷��
		Kind GetFileKind(const std::wstring& name);

	public:

		// �R���X�g���N�^
		ResourceManager(ID3D11Device* device, DirectX::WaveBank* waveBank);

		// �e�N�X�`���쐬�֐�
		ID3D11ShaderResourceView* CreateTexture(const wchar_t* name);

		// �e�N�X�`���̉���֐�
		void ReleaseTexture(const wchar_t* name);

		// ���f���쐬�֐�
		DirectX::Model* CreateModel(const wchar_t* name);

		// ���f���̉���֐�
		void ReleaseModel(const wchar_t* name);

		// �T�E���h�G�t�F�N�g�C���X�^���X�쐬�֐�
		DirectX::SoundEffectInstance* CreateSoundEffectInstance(unsigned int index);

		// �T�E���h�G�t�F�N�g�C���X�^���X�̉���֐�
		void ReleaseSoundEffect(unsigned int index);
	};
}
