#include "SM_Common.hlsli"

// シャドウマップテクスチャ
Texture2D ShadowMapTexture : register(t1);

// シャドウマップ用テクスチャサンプラー
SamplerComparisonState ShadowMapSampler : register(s1);

struct Input
{
    float2 TexCoord     : TEXCOORD0;
    float4 PositionWS   : TEXCOORD1;
    float3 NormalWS     : TEXCOORD2;
    float4 LightPosPS   : TEXCOORD3;
};

// マッハバンド対策
#define SHADOW_EPSILON 0.0005f

float4 main(Input pin) : SV_TARGET0
{
    float4 diffuse;
    float3 specular = 0;

    // ライトの向きを求める
    float3 lightDir = normalize(pin.PositionWS.xyz - LightPos.xyz);

    // スポットライトの範囲内か？
    if (dot(lightDir, LightDir) > fCosTheta)
    {
        // 正規化デバイス座標にする
        pin.LightPosPS.xyz /= pin.LightPosPS.w;

        // 参照するシャドウマップのUV値を求める
        float2 uv = pin.LightPosPS.xy * float2(0.5f, -0.5f) + 0.5f;

        // シャドウマップの深度値とライト空間のピクセルのZ値を比較して影になるか調べる
        float percentLit = ShadowMapTexture.SampleCmpLevelZero(ShadowMapSampler, uv, pin.LightPosPS.z - SHADOW_EPSILON).x;
 
        // ------------------------------------------------------------------------------- //
        // ディフューズ
        // ------------------------------------------------------------------------------- //
 
        // 法線を正規化
        float3 worldNormal = normalize(pin.NormalWS);

        // 光の強さを内積から算出する
        float3 dotL = saturate(dot(-lightDir, worldNormal));

        // ライトによる明るさを求める
        float3 lightAmount = dotL * percentLit * (1.0f - LightAmbient) + LightAmbient;

        // ディフューズ色を求める 
        diffuse = float4(DiffuseColor.rgb * lightAmount, DiffuseColor.a);

        // ------------------------------------------------------------------------------- //
        // スペキュラ
        // ------------------------------------------------------------------------------- //
    
        // 視線ベクトル
        float3 eyeVector = normalize(EyePosition - pin.PositionWS.xyz);

        // ハーフベクトル
        float3 halfVector = normalize(eyeVector - lightDir);

        // スペキュラの影響割合を内積を使い求める
        float dotH = saturate(dot(halfVector, worldNormal));

        // スペキュラパワーを指数として使いハイライトのかかり具合を調整
        specular = pow(dotH, SpecularPower) * dotL * SpecularColor * percentLit;
    }
    else
    {
        // スポットライトの外
        diffuse = float4(LightAmbient, 1.0f) * DiffuseColor;
    }
  
    // テクスチャ色とディフューズ色を掛ける 
    float4 color = Texture.Sample(Sampler, pin.TexCoord) * diffuse;

    // スペキュラを加える
    color.rgb += specular * diffuse.a;

    return color;
}

