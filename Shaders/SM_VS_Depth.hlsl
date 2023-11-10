#include "SM_Common.hlsli"

struct VSInputNmTxVc
{
    float4 Position     : SV_Position;
    float3 Normal       : NORMAL;
    float2 TexCoord     : TEXCOORD0;
    float4 Color        : COLOR;
};

struct Output
{
    float2 Depth        : TEXCOORD0;
    float4 PositionPS   : SV_Position;
};

Output main(VSInputNmTxVc vin)
{
    Output output;
    
    output.PositionPS = mul(vin.Position, WorldViewProj);
    output.Depth = output.PositionPS.zw;

    return output;
}
