Texture2D<float> depthTex : register(t0);
SamplerState smp : register(s0);

struct PSInput
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

float4 main(PSInput input) : SV_TARGET
{
	return float4(input.pos.z, input.pos.z, input.pos.z, 1.0);
	//return float4(input.uv, 0.0f, 1.0f);
	float depth = depthTex.Sample(smp, input.uv);
    return float4(depth, depth, depth, 1.0);
}