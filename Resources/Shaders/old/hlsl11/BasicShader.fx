//uniforms
cbuffer cbPerObject
{
	float4x4 mMV;
	float4x4 mProj;
};

cbuffer cbPerFrame
{
	float4	 vLightPos; // light position
	float4	 vEye; //camera look-at vector, used in the phong specular model
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
	float3 outPosWorld : POSITION;
	float3 outNormalWorld : NORMAL;
};

//actual vertex shader
VertexOutputType FlatVertexShader(VertexInputType)
{
	VertexOutputType output;
	
	//make sure 4th vector in element is 1
	
	output.pos = mul(float4(input.pos,1), mMVP);
	output.col = input.col;
	
	return output;
}

//actual pixel shader
float4 FlatPixelShader(VertexOutputType input) : SV_TARGET
{ 
	return input.col;
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

