out vec4 vFragColor;

uniform samplerCube skyMap;
smooth in vec3 vTexCoord;

void main(void)
{ 
	vFragColor = texture(skyMap, vTexCoord.stp);
}
