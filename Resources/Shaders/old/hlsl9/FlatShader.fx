//background color, ARGB
int BCLR = 0xff000000;  //really necessary?

//uniforms
float4x4 mMVP : WORLDVIEWPROJ;
float4 vColor : COLOR0;

//in and out definitions
struct VS_IN
{
	float3 pos : POSITION;
	float3 nor : NORMAL;
	float2 uv : TEX_COORD;
	float4 tan : TANGENT;
	float3 bitan : BINORMAL; //??
};

struct VS_OUTPUT
{
	float4 pos : POSITION;
	float4 col : COLOR0;
};

//vertex shader
VS_OUTPUT VS(VS_IN input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.pos = mul(float4(input.pos,1), mMVP);
	output.col = vColor;
	return output;
}

//pixel shader
float4 PS(VS_OUTPUT input) : COLOR
{
	return input.col;
}

//technique
technique FlatShader
{
	pass P0
	{
		//should probably need to updated the shader model number. 
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile ps_3_0 PS();
	}
}

