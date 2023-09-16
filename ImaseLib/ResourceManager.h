//--------------------------------------------------------------------------------------
// File: ResourceManager.h
//
// リソースを管理するクラス
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

		// ファイルの種類
		enum class Kind
		{
			UNKNOWN,
			DDS,
			CMO,
			WAV,
		};

		// デバイス
		ID3D11Device* m_device;

		// ウェーブバンクへのポインタ
		DirectX::WaveBank* m_waveBank;

		// テクスチャキャッシュ
		using TextureCache = std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>;
		TextureCache m_textureCache;

		// モデルキャッシュ
		using ModelCache = std::unordered_map<std::wstring, std::unique_ptr<DirectX::Model>>;
		ModelCache m_modelCache;

		// サウンドキャッシュ
		using SoundCache = std::unordered_map<unsigned int, std::unique_ptr<DirectX::SoundEffectInstance>>;
		SoundCache m_soundCache;

		// ファイル名から種類を判定する
		Kind GetFileKind(const std::wstring& name);

	public:

		// コンストラクタ
		ResourceManager(ID3D11Device* device, DirectX::WaveBank* waveBank);

		// テクスチャ作成関数
		ID3D11ShaderResourceView* CreateTexture(const wchar_t* name);

		// テクスチャの解放関数
		void ReleaseTexture(const wchar_t* name);

		// モデル作成関数
		DirectX::Model* CreateModel(const wchar_t* name);

		// モデルの解放関数
		void ReleaseModel(const wchar_t* name);

		// サウンドエフェクトインスタンス作成関数
		DirectX::SoundEffectInstance* CreateSoundEffectInstance(unsigned int index);

		// サウンドエフェクトインスタンスの解放関数
		void ReleaseSoundEffect(unsigned int index);
	};
}
