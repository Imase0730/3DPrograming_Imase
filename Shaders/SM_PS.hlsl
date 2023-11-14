#include "SM_Common.hlsli"

// シャドウマップテクスチャ
Texture2D ShadowMapTexture : register(t1);

// シャドウマップ用テクスチャサンプラー
sampler ShadowMapSampler : register(s1);

struct Input
{
    float2 TexCoord     : TEXCOORD0;
    float4 PositionWS   : TEXCOORD1;
    float3 NormalWS     : TEXCOORD2;
    float4 LightPosPS   : TEXCOORD3;
};

// マッハバンド対策
#define SHADOW_EPSILON 0.00005f

float4 main(Input pin) : SV_TARGET0
{
    // ライトの向きを求める
    float3 lightDir = normalize(pin.PositionWS.xyz - LightPos.xyz);

    // 正規化デバイス座標にする
    pin.LightPosPS.xyz /= pin.LightPosPS.w;

    // 参照するシャドウマップのUV値を求める
    float2 uv = pin.LightPosPS.xy * float2(0.5f, -0.5f) + 0.5f;

    // 描画するピクセルが影か判定する
    float bright = (ShadowMapTexture.Sample(ShadowMapSampler, uv).x + SHADOW_EPSILON < pin.LightPosPS.z) ? 0.0f : 1.0f;

    // ディフューズ色を求める
    float4 diffuse = saturate(dot(-lightDir, normalize(pin.NormalWS)) * bright * (1.0f - LightAmbient) + LightAmbient) * DiffuseColor;

    // テクスチャ色を掛ける 
    float4 color = Texture.Sample(Sampler, pin.TexCoord) * diffuse;
    
    return color;
}

