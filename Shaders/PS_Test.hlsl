
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

struct PSInputPixelLightingTx
{
    float2 TexCoord : TEXCOORD0;
    float4 PositionWS : TEXCOORD1;
    float3 NormalWS : TEXCOORD2;
    float4 Diffuse : COLOR0;
};

// Pixel shader: pixel lighting.
float4 PSBasicPixelLighting(PSInputPixelLightingTx pin) : SV_Target0
{
    // �e�N�X�`���F �~ Vc���w�肳��Ă���ꍇ�͒��_�J���[ + �A���t�@
    float4 color = Texture.Sample(Sampler, pin.TexCoord) * pin.Diffuse;

    // �@���𐳋K��
    float3 worldNormal = normalize(pin.NormalWS);

    // ���̋�������ς���Z�o����
    float3 dotL = dot(-LightDirection[0], worldNormal);
    
    // �\�ʂ̏ꍇ�͂P�A���ʂ̏ꍇ��0
    float3 zeroL = step(0, dotL);
    
    // ���ʂ̏ꍇ�͍��ɂȂ�
    float3 diffuse = zeroL * dotL;
    
    // ���̉e�� �~ �}�e���A���̃f�B�t���[�Y�F �{ �A���r�G���g�F�i�G�~�b�V�u�F���܂ށj
    diffuse = diffuse * DiffuseColor.rgb + EmissiveColor;
    
    // ���̉e�����|����
    color.rgb *= diffuse;
    
    return color;
}