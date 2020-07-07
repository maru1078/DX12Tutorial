
cbuffer ConstantBuffer : register(b0)
{
	float4 col;
};

struct VSInput
{
	float2 pos : POSITION;
};

struct VSOutput
{
	float4 pos : SV_POSITION;
	float4 col : COLOR;
};

VSOutput main(VSInput input)
{
	VSOutput output;
	output.pos = float4(input.pos, 0.0, 1.0); // w�̒l�͉���\���Ă���H 1.0�ɂ���Ƃ����ƕ`�悳���
	output.pos.xy = output.pos.xy * (float2(2.0 / 960.0, 2.0 / -540.0)) + float2(-1.0, 1.0);
	output.col = col;

	return output;
}