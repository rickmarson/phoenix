in vec3 vVertex;
in vec3 vNormal;

uniform mat4 mMV;
uniform mat4 mProj;
uniform mat4 mView; //it's the inverse of the camera model matrix

// Texture coordinate to fragment program
smooth out vec3 vTexCoord;

void main(void) 
{
	// Normal in Eye Space
	mat3 mNormalMatrix;
	mNormalMatrix[0] = normalize(mMV[0].xyz);
	mNormalMatrix[1] = normalize(mMV[1].xyz);
	mNormalMatrix[2] = normalize(mMV[2].xyz);
	vec3 vEyeNormal = normalize(mNormalMatrix * vNormal);
   
    // Vertex position in Eye Space
	vec4 vVert4 = vec4(vVertex, 1.0);
    vec4 tmp = mMV * vVert4;
    vec3 vEyeVertex = normalize(tmp.xyz / tmp.w);
    
	// Get reflected vector
	vec4 vCoords = vec4(reflect(vEyeVertex, vEyeNormal), 1.0);
   
	// Rotate by flipped camera
	vCoords = mView * vCoords;
	vTexCoord.xyz = normalize(vCoords.xyz);

	mat4 mMVP = mProj * mMV;
	gl_Position = mMV * vVert4;
}
