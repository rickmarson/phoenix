//background color, ARGB
int BCLR = 0xff000000;  //really necessary?

//uniforms
float4x4 mModel : WORLD;
float4x4 mView : VIEW;
float4x4 mProj : PROJECTION;
float4 vColor : COLOR0;

//in and out definitions
struct VS_IN
{
	float3 pos : POSITION;
	float3 nor : NORMAL;
	float2 uv : TEX_COORD;
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
	float4x4 WorldView = mul(mModel, mView);
	float4x4 MVP = mul(WorldView, mProj);
	output.pos = mul(float4(input.pos,1), MVP);
	output.col = vColor;
	return output;
}

//pixel shader
float4 PS(VS_OUTPUT input) : COLOR
{
	float4 color;
	color.x = 1.0;
	color.y = 0.0;
	color.z = 0.5;
	color.w = 1.0;

	//return input.col;
	return color;
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

