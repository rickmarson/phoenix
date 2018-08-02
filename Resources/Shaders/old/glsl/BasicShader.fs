smooth in vec3 vLightDir;
smooth in vec3 vNormalOut;
in vec3 vEyeDir; //already normalised in the vertex shader
in vec2 vTex;

//material properties
uniform vec4 vDiffuseColor;
uniform vec4 vAmbientColor;
uniform vec4 vSpecularColor;
uniform float sShininess;
//texture
uniform sampler2D diffuseTexture0;
//light properties
uniform vec4 vLightDiffuse;
uniform vec4 vLightAmbient;
uniform vec4 vLightSpecular;

out vec4 FragColor;

void main(void) 
{ 
	vec4 LightColor;

	//diffuse component
	float fDot = max(0.0, dot(vNormalOut, vLightDir));
	vec3 vTotalLightDiffuse = vLightDiffuse.rgb * fDot;
	LightColor.rgb = vDiffuseColor.rgb * vTotalLightDiffuse;

	//add ambient component
	LightColor.rgb += vAmbientColor.rgb * vLightAmbient.rgb;

	//add specular component
	vec3 reflect = normalize(2.0*vTotalLightDiffuse*vNormalOut - vLightDir);
	vec3 vTotalLightSpec = vLightSpecular.rgb * pow( max(0.0, dot(reflect, vEyeDir) ), sShininess );
	LightColor.rgb += vSpecularColor.rgb * vTotalLightSpec;

	//copy alpha value
	LightColor.a = vDiffuseColor.a;
	
	//modulate the texture color with the total light color computed in the VS
	//LighColor contains both light intensity and color and the base mesh color.
	FragColor = LightColor * texture2D(diffuseTexture0, vTex);
}
