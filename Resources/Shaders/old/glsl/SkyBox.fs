in vec3 vTexCoord;

uniform samplerCube skyMap;

out vec4 vFragColor;

void main(void)
{ 
	vFragColor = texture(skyMap, vTexCoord);
}
