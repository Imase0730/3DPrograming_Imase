//--------------------------------------------------------------------------------------
// File: ShadowMap.h
//
// シャドウマップクラス
//
// Date: 2023.9.4
// Author: Hideyasu Imase
//--------------------------------------------------------------------------------------
#pragma once

#include "SimpleMath.h"
#include "CommonStates.h"

namespace Imase
{
	class ShadowMap
	{
	private:

		// シャドウマップの解像度
		static const UINT DEFALT_RESOLUTION = 2048;

		// 深度バイアス（初期値 0.1 バイアス値の計算は m_offset / ライトの影響範囲）
		static const float DEFALT_OFFSET;

		// シャドウマップ作成時のニアークリップの値
		static const float NEAR_CLIP;

		// csoの場所
		std::wstring m_path;

		// シャドウマップの解像度
		UINT m_resolution;

		// ガウシアンフィルタ
		bool m_gaussianFilterEnable;

		// 分散シャドウ
		bool m_vsmFilterEnable;

		// 深度オフセット
		float m_offset;

		// 頂点シェーダー
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VS_ShadowMap;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VS_ShadowModel;

		// ピクセルシェーダー
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PS_ShadowMap;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PS_ShadowModel;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PS_ShadowModel_Texture;

		// 深度バッファ用テクスチャ
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depathTexture2D;

		// シャドウマップ用テクスチャ
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_shadowMapTexture2D;

		// 深度バッファビュー
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DSV;

		// シャドウマップ用のレンダーターゲットビュー
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_shadowMapRTV;

		// シャドウマップ用のシェーダーリソースビュー
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shadowMapSRV;

		// シャドウマップ用の入力レイアの入力レイアウトウト
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_shadowMapInputLayout;

		// ビューポート【0:シャドウマップ用】
		D3D11_VIEWPORT m_viewPort;

		// 定数バッファの構造体の定義
		struct ConstantBuffer
		{
			DirectX::XMMATRIX lightViewProj;
			DirectX::XMFLOAT3 lightPosition;
			float maxDepth;
			float offset;
			float VSMFilterEnable;
			float test;
		};

		// 定数バッファ
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_shadowMapConstantBuffer;

		// サンプラー
		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler;

		// 頂点バッファ
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBufferGF;

	private:

		// csoの入っているディレクトの設定関数
		void SetDirectory(const wchar_t* path);

		// パス付きの名前に変換する関数
		const wchar_t* GetFullName(const wchar_t* name)
		{
			static std::wstring fullName;

			fullName = m_path;
			fullName += name;

			return fullName.c_str();
		}

		// 上方向ベクトルを算出する関数
		DirectX::SimpleMath::Vector3 CalcUpVector(const DirectX::SimpleMath::Vector3& v);

	public:

		// コンストラクタ
		ShadowMap();

		// 初期化関数
		void Initialize(ID3D11Device* device, const wchar_t* path = nullptr, UINT resolution = DEFALT_RESOLUTION);

		// シャドウマップ作成開始
		// maxDepthの値はライトの位置からライトの影響を受ける範囲
		void Begin(ID3D11DeviceContext* context, DirectX::SimpleMath::Vector3 lightPos, DirectX::SimpleMath::Vector3 targetPos, float maxDepth);

		// シャドウマップ作成時に呼び出すカスタムステート
		void DrawShadowMap(ID3D11DeviceContext* context);

		// 影付きのモデルを描画したい時に呼び出すカスタムステート
		// テクスチャなしのモデルを表示したい場合は第２引数をfalseにする事
		void DrawShadow(ID3D11DeviceContext* context, bool texture);

		// シャドウマップ用テクスチャリソースを取得する関数（デバッグ用）
		ID3D11ShaderResourceView* GetShadowMapTexture()
		{
			return m_shadowMapSRV.Get();
		}

		// 深度バッファの比較オフセット値の設定（ぼかした場合はオフセット値は無効）
		void SetOffset(float offset)
		{
			m_offset = offset;
		}

	};
}

