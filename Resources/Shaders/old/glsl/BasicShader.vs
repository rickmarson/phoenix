//attributes
in vec3 vVertex;
in vec3 vNormal;
in vec2 vTexCoord0;

//uniforms
//transform matrices
uniform mat4 mMV;
uniform mat4 mProj;
//light position
uniform vec4 vLightPos;
//camera look-at vector, used in the phong specular model
uniform vec4 vEye;

//outputs
smooth out vec3 vLightDir;
smooth out vec3 vNormalOut;
out vec3 vEyeDir;
out vec2 vTex;

void main(void) 
{ 
	vec4 Vertex = vec4(vVertex, 1.0);

	mat3 mNormalMatrix;
	mNormalMatrix[0] = normalize(mMV[0].xyz);
	mNormalMatrix[1] = normalize(mMV[1].xyz);
	mNormalMatrix[2] = normalize(mMV[2].xyz);
	vNormalOut = normalize(mNormalMatrix * vNormal);
	
	vec3 ecPosition3;
	vec4 temp;

	ecPosition3 = vec3(mMV * Vertex);
	temp = mMV * vLightPos;
	vLightDir = normalize(vec3(temp) - ecPosition3);
	temp = mMV * vEye;
	vEyeDir = normalize(vec3(temp));
	
	mat4 mMVP;
	mMVP = mProj * mMV;
	gl_Position = mMVP * Vertex; 
	vTex = vTexCoord0;
}
 
