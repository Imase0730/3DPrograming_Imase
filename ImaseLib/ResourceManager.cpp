//--------------------------------------------------------------------------------------
// File: ResourceManager.cpp
//
// ���\�[�X���Ǘ�����N���X
//
// Date: 2023.8.24
// Author: Hideyasu Imase
//--------------------------------------------------------------------------------------
#include "pch.h"
#include "ResourceManager.h"
#include "ReadData.h"

using namespace DirectX;
using namespace Imase;

// �R���X�g���N�^
Imase::ResourceManager::ResourceManager(ID3D11Device* device, DirectX::WaveBank* waveBank)
    : m_device(device), m_waveBank(waveBank)
{
}

// �e�N�X�`���쐬�֐�
ID3D11ShaderResourceView* Imase::ResourceManager::CreateTexture(const wchar_t* name)
{
    auto it = m_textureCache.find(name);

    // �L���b�V���ɑ��݂��Ȃ�����
    if (it == m_textureCache.end())
    {
        // �e�N�X�`���̍쐬
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
        DX::ThrowIfFailed(CreateDDSTextureFromFile(m_device, name, nullptr, srv.GetAddressOf()));
        TextureCache::value_type v(name, srv);
        m_textureCache.insert(v);
    }

    return m_textureCache.at(name).Get();
}

// �e�N�X�`���̉���֐�
void Imase::ResourceManager::ReleaseTexture(const wchar_t* name)
{
    if (!m_textureCache.count(name)) return;

    m_textureCache.erase(name);
}

// ���f���쐬�֐�
DirectX::Model* Imase::ResourceManager::CreateModel(const wchar_t* name)
{
    auto it = m_modelCache.find(name);

    // �L���b�V���ɑ��݂��Ȃ�����
    if (it == m_modelCache.end())
    {
        wchar_t path[MAX_PATH] = {};
        _wsplitpath_s(name, nullptr, 0, path, MAX_PATH, nullptr, 0, nullptr, 0);
        EffectFactory fx(m_device);
        fx.SetDirectory(path);
        // ���f���̍쐬
        std::unique_ptr<DirectX::Model> model = Model::CreateFromCMO(m_device, name, fx);
        ModelCache::value_type v(name, std::move(model));
        m_modelCache.insert(std::move(v));
    }

    return m_modelCache.at(name).get();
}

// ���f���̉���֐�
void Imase::ResourceManager::ReleaseModel(const wchar_t* name)
{
    if (!m_modelCache.count(name)) return;

    m_modelCache.erase(name);
}

// �T�E���h�G�t�F�N�g�쐬�֐�
DirectX::SoundEffectInstance* Imase::ResourceManager::CreateSoundEffectInstance(unsigned int index)
{
    auto it = m_soundCache.find(index);

    // �L���b�V���ɑ��݂��Ȃ�����
    if (it == m_soundCache.end())
    {
        // �T�E���h�G�t�F�N�g�̍쐬
        std::unique_ptr<SoundEffectInstance> wb = m_waveBank->CreateInstance(index);
        if (wb)
        {
            SoundCache::value_type v(index, std::move(wb));
            m_soundCache.insert(std::move(v));
        }
        else
        {
            OutputDebugString(L"invalid index for wave bank\n");
        }
    }

    return m_soundCache.at(index).get();
}

// �T�E���h�G�t�F�N�g�̉���֐�
void Imase::ResourceManager::ReleaseSoundEffect(unsigned int index)
{
    if (!m_soundCache.count(index)) return;

    m_soundCache.erase(index);
}

#if 0
// �t�@�C���̎�ނ��擾����֐�
ResourceManager::Kind ResourceManager::GetFileKind(const std::wstring& name)
{
    size_t dotPos = name.rfind(L'.');
    if (dotPos == std::wstring::npos)
    {
        // '.'�����݂��Ȃ��ꍇ
        return Kind::UNKNOWN;
    }

    // �g���q�̔����o��
    std::wstring extension = name.substr(dotPos + 1);

    // �g���q���������ɕς���
    std::transform(extension.begin(), extension.end(), extension.begin(), std::towlower);

    // �g���q�𔻒肷��
    if (extension == L"dds") return Kind::DDS;
    if (extension == L"cmo") return Kind::CMO;

    return Kind::UNKNOWN;
}
#endif
