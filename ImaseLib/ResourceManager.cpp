//--------------------------------------------------------------------------------------
// File: ResourceManager.cpp
//
// リソースを管理するクラス
//
// Date: 2023.8.24
// Author: Hideyasu Imase
//--------------------------------------------------------------------------------------
#include "pch.h"
#include "ResourceManager.h"
#include "ReadData.h"

using namespace DirectX;
using namespace Imase;

// コンストラクタ
Imase::ResourceManager::ResourceManager(ID3D11Device* device, DirectX::WaveBank* waveBank)
    : m_device(device), m_waveBank(waveBank)
{
}

// テクスチャ作成関数
ID3D11ShaderResourceView* Imase::ResourceManager::CreateTexture(const wchar_t* name)
{
    auto it = m_textureCache.find(name);

    // キャッシュに存在しなかった
    if (it == m_textureCache.end())
    {
        // テクスチャの作成
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
        DX::ThrowIfFailed(CreateDDSTextureFromFile(m_device, name, nullptr, srv.GetAddressOf()));
        TextureCache::value_type v(name, srv);
        m_textureCache.insert(v);
    }

    return m_textureCache.at(name).Get();
}

// テクスチャの解放関数
void Imase::ResourceManager::ReleaseTexture(const wchar_t* name)
{
    if (!m_textureCache.count(name)) return;

    m_textureCache.erase(name);
}

// モデル作成関数
DirectX::Model* Imase::ResourceManager::CreateModel(const wchar_t* name)
{
    auto it = m_modelCache.find(name);

    // キャッシュに存在しなかった
    if (it == m_modelCache.end())
    {
        wchar_t path[MAX_PATH] = {};
        _wsplitpath_s(name, nullptr, 0, path, MAX_PATH, nullptr, 0, nullptr, 0);
        EffectFactory fx(m_device);
        fx.SetDirectory(path);
        // モデルの作成
        std::unique_ptr<DirectX::Model> model = Model::CreateFromCMO(m_device, name, fx);
        ModelCache::value_type v(name, std::move(model));
        m_modelCache.insert(std::move(v));
    }

    return m_modelCache.at(name).get();
}

// モデルの解放関数
void Imase::ResourceManager::ReleaseModel(const wchar_t* name)
{
    if (!m_modelCache.count(name)) return;

    m_modelCache.erase(name);
}

// サウンドエフェクト作成関数
DirectX::SoundEffectInstance* Imase::ResourceManager::CreateSoundEffectInstance(unsigned int index)
{
    auto it = m_soundCache.find(index);

    // キャッシュに存在しなかった
    if (it == m_soundCache.end())
    {
        // サウンドエフェクトの作成
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

// サウンドエフェクトの解放関数
void Imase::ResourceManager::ReleaseSoundEffect(unsigned int index)
{
    if (!m_soundCache.count(index)) return;

    m_soundCache.erase(index);
}

#if 0
// ファイルの種類を取得する関数
ResourceManager::Kind ResourceManager::GetFileKind(const std::wstring& name)
{
    size_t dotPos = name.rfind(L'.');
    if (dotPos == std::wstring::npos)
    {
        // '.'が存在しない場合
        return Kind::UNKNOWN;
    }

    // 拡張子の抜き出し
    std::wstring extension = name.substr(dotPos + 1);

    // 拡張子を小文字に変える
    std::transform(extension.begin(), extension.end(), extension.begin(), std::towlower);

    // 拡張子を判定する
    if (extension == L"dds") return Kind::DDS;
    if (extension == L"cmo") return Kind::CMO;

    return Kind::UNKNOWN;
}
#endif
