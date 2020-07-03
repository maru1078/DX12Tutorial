cbuffer ConstantBuffer : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
	float3 position;
	float  radius;
	float4 color;
};

struct VSInput
{
	float4 position : POSITION;
};

struct VSOutput
{
	float4 pos : SV_POSITION;
	float4 col : COLOR;
};

VSOutput main(VSInput input)
{
	input.position.xyz *= radius; // ”¼Œa‚ð“K—p
	float4 tmpInputPosition = float4(input.position.xyz, 1.0);
	VSOutput output;
	float4 worldPosition = mul(world, tmpInputPosition);
	float4 viewPosition = mul(view, worldPosition);
	output.pos = mul(projection, viewPosition);
	output.col = color;

	return output;
}