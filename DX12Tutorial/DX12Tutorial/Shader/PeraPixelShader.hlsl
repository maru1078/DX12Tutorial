Texture2D<float> depthTex : register(t0);
SamplerState smp : register(s0);

struct PSInput
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

float4 main(PSInput input) : SV_TARGET
{
	float depth = pow(depthTex.Sample(smp, input.uv), 20);
    return float4(depth, depth, depth, 1.0);
}