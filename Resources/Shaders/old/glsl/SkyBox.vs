in vec3 vVertex;

uniform mat4 mMVP;

out vec3 vTexCoord;

void main(void) 
{
	vTexCoord = normalize(vVertex);
	gl_Position = mMVP * vec4(vVertex, 1.0); 
}
