begin vertex

    vec3 vVertex;

end

begin uniforms

    mat4 mMVP;
    samplerCube skyMap;

end

begin vertex_outputs

    vec3 vTexCoord;

end

begin vertex_shader

	vTexCoord = normalize(vVertex);
	VertexOut = mMVP * vec4(vVertex, 1.0); 

end

begin fragment_shader

    ColorOut = texture(skyMap, vTexCoord);

end
