cbuffer CB0
{
	matrix WVPMatrix; //64 bytes
	float red_fraction; // 4 bytes
	float scale; //4 bytes
	float2 packing; //2x4 bytes
}; //80 bytes

TextureCube cube0;
SamplerState sampler0;

struct VOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float3 texcoord : TEXCOORD;
};

VOut ModelVS(float4 position : POSITION, float3 texcoord : TEXCOORD, float3 normal : NORMAL)
{
	VOut output;

	float4 default_color = { 1,1,1,1 };

	output.position = mul(WVPMatrix, position);
	output.texcoord = position.xyz;
	output.color = default_color;

	return output;
}

float4 ModelPS(float4 position : SV_POSITION, float4 color : COLOR, float3 texcoord : TEXCOORD) : SV_TARGET
{
	return cube0.Sample(sampler0, texcoord) * color;
}