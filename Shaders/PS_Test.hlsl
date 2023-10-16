//float4 main() : SV_TARGET
//{
//	return float4(1.0f, 1.0f, 1.0f, 1.0f);
//}

struct PSInputNoFog
{
	float4 Diffuse : COLOR0;
};

float4 PSBasicNoFog(PSInputNoFog pin) : SV_Target0
{
	return pin.Diffuse;
}
