cbuffer CB0
{
	matrix WorldViewMatrix; //64 bytes
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

	//position relative to camera
	float3 wvpos = mul(WorldViewMatrix, position);

	//surface normal relative to camera
	float3 wvnormal = mul(WorldViewMatrix, normal);
	wvnormal = normalize(wvnormal);

	//obtain the reverse eye vector
	float3 eyer = -normalize(wvpos);

	//compute the reflection vector
	output.texcoord = 2.0 * dot(eyer, wvnormal) * wvnormal - eyer;

	return output;
}

float4 ModelPS(float4 position : SV_POSITION, float4 color : COLOR, float3 texcoord : TEXCOORD) : SV_TARGET
{
	return cube0.Sample(sampler0, texcoord) * color;
}