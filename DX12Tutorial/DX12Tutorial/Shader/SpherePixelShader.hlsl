cbuffer ConstantBuffer : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
	float4 position;
	float4 color;
};

struct PSInput
{
	float4 pos : SV_POSITION;
	float4 col : COLOR;
};

float4 main(PSInput input) : SV_TARGET
{
	//return float4(input.pos.z, input.pos.z, input.pos.z, 1.0);
	return input.col;
}
