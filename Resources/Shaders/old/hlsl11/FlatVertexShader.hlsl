//uniforms
cbuffer cbPerObject
{
	float4x4 mMVP;
	float3 vDiffuseColor;
};

//inputs and ouputs
struct VertexInputType
{
	float3 inPos : POSITION;
	float3 inNorm : NORMAL;
	float2 inTexCoord : TEXCOORD;
};

struct VertexOutputType //==PixelInput
{
	float4 outPos : SV_POSITION;
	float3 outCol : COLOR;
};

//actual vertex shader
void main(in VertexInputType input, out VertexOutputType output )
{	
	//make sure 4th vector in element is 1
	output.outPos = mul(float4(input.inPos, 1.0f), mMVP);
	output.outCol = vDiffuseColor;
}
