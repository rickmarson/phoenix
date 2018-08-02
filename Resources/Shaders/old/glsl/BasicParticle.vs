in vec3 vVertex;
in vec4 vVelocity;

uniform mat4 mMV;
uniform mat4 mProj;

out vec4 Vel;

void main(void) 
{
	Vel = vVelocity;
	mat4 MVP = mProj * mMV;
	gl_Position = MVP * vec4(vVertex, 1.0); 
}