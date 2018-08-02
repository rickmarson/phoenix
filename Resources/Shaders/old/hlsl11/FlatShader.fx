//uniforms
cbuffer cbPerObject
{
	float4x4 mMVP;
	float3 color;
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
};

//actual vertex shader
VertexOutputType FlatVertexShader(VertexInputType input)
{
	VertexOutputType output;
	
	//make sure 4th vector in element is 1
	output.outPos = mul(float4(input.inPos, 1.0f), mMVP);
	
	return output;
}

//actual pixel shader
float4 FlatPixelShader(VertexOutputType input) : SV_TARGET
{ 
	return color;
}

//technique
technique11 FlatShader
{
	pass P0
	{ 
		SetVertexShader( CompileShader( vs_5_0, FlatVertexShader() ));
		SetPixelShader( CompileShader( ps_5_0, FlatPixelShader() ));
	}
}

