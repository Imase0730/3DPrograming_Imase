
Texture2D<float4> Texture : register(t0);
sampler Sampler : register(s0);


cbuffer Parameters : register(b0)
{
    float4 DiffuseColor             : packoffset(c0);
    float3 EmissiveColor            : packoffset(c1);
    float3 SpecularColor            : packoffset(c2);
    float  SpecularPower : packoffset(c2.w);

    float3 LightDirection[3]        : packoffset(c3);
    float3 LightDiffuseColor[3]     : packoffset(c6);
    float3 LightSpecularColor[3]    : packoffset(c9);

    float3 EyePosition              : packoffset(c12);

    float3 FogColor                 : packoffset(c13);
    float4 FogVector                : packoffset(c14);

    float4x4 World                  : packoffset(c15);
    float3x3 WorldInverseTranspose  : packoffset(c19);
    float4x4 WorldViewProj          : packoffset(c22);
};

//float4 main() : SV_TARGET
//{
//	return float4(1.0f, 1.0f, 1.0f, 1.0f);
//}

struct PSInputPixelLighting
{
    float4 PositionWS : TEXCOORD0;
    float3 NormalWS   : TEXCOORD1;
    float4 Diffuse    : COLOR0;
};

// Pixel shader: pixel lighting.
float4 PSBasicPixelLighting(PSInputPixelLighting pin) : SV_Target0
{
    // 法線を正規化
   float3 worldNormal = normalize(pin.NormalWS);

   // 光の強さを内積から算出する
   float3 dotL = dot(-LightDirection[0], worldNormal);

   // 表面の場合は１、裏面の場合は0
   float3 zeroL = step(0, dotL);

   // 裏面の場合は黒になる
   float3 diffuse = zeroL * dotL;

   // マテリアルのディフューズ色を掛ける
   diffuse = diffuse * DiffuseColor.rgb;

   return float4(diffuse, 1.0f);
}
