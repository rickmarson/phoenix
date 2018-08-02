//transform matrices
uniform float4x4 mMV;
uniform float4x4 mProj;
//material properties
uniform float4 vDiffuseColor;
uniform float4 vAmbientColor;
uniform float4 vSpecularColor;
uniform float sShininess;
//light properties
uniform float4 vLightPos; //float3?
uniform float4 vLightDiffuse;
uniform float4 vLightAmbient;
uniform float4 vLightSpecular;
//camera look-at vector, used in the phong specular model
uniform float4 vEye;
//textures
uniform sampler2D diffuseTexture0 : TEXUNIT0;

struct VS_INPUT
{
	float4 vPos : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float4 tan : TANGENT;
	float3 bitan : BINORMAL; //??
};

struct VS_OUTPUT
{
	float4 vPosition : POSITION;
	float4 vColor : COLOR0;
	float2 vTexUV : TEXCOORD0;
};

VS_OUTPUT BasicShaderVS(VS_INPUT input)
{
	VS_OUTPUT output;

	//transform the normal from object space to clip space
	float3 vNorm = normalize( mul(input.vNormal, mMV ) );
	//calc MVP matrix and output position
	float4x4 mMVP = mul(mProj, mMV); //check order
	output.vPosition = mul(input.vPos, mMVP);
	//pass vertex tex coordinates to next stage
	output.vTexUV = input.vTexUV;
	//vLightPos is supposed to be passed in already in clip space
	//colored light with phong specular reflection
	float3 vTotalLightDiffuse = vLightDiffuse * max(0, dot(vNorm, vLightPos) ); //check dot order
	output.vColor.rgb = vDiffuseColor.rgb * vTotalLightDiffuse;
	//add ambient component
	output.vColor.rgb += vAmbientColor * vLightAmbient;
	//add specular component
	float3 reflect = normalize(2*vTotalLightDiffuse*vNorm - vLightPos);
	float3 vTotalLightSpec = vLightSpecular * pow( max(0, dot(reflect, normalize(vEye) )), sShininess ); //make sure vEye is normalized
	output.vColor.rgb += vSpecularColor * vTotalLightSpec;
	//copy alpha value
	output.vDiffuseColor.a = vDiffuseColor.a;

	return output;
}

float4 BasicShaderPS(PS_INPUT input)
{
	return (tex2D(diffuseTexture0, input.vTexUV) * input.vDiffuseColor);
}

technique BasicShader
{
	pass P0
	{
		VertexShader = compile vs_3_0 BasicShaderVS();
		PixelShader = compile ps_3_0 BasicShaderPS();
	}
}
