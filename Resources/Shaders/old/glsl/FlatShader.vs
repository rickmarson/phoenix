in vec3 vVertex;
uniform mat4 mMVP;

void main(void) 
{
	gl_Position = mMVP * vec4(vVertex, 1.0); 
}
