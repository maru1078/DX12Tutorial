cbuffer ConstantBuffer : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
	float4 position;
	float4 color;
};

struct VSInput
{
	float4 position : INPOSITION;
};

struct VSOutput
{
	float4 pos : SV_POSITION;
	float4 col : COLCOL;
};

VSOutput main(VSInput input)
{
	float4 tmpInputPosition = float4(input.position.xyz, 1.0);
	VSOutput output;
	//input.position += position;
	float4 worldPosition = mul(world, tmpInputPosition);
	float4 viewPosition = mul(view, worldPosition);
	output.pos = mul(projection, viewPosition);
	//output.pos = mul(mul(mul(projection, view), world), input.position);
	output.col = color;
	//output.pos.z = 0.5;
	//output.pos.w = 1.0;

	return output;
}