begin vertex

    vec2 vVertex;
    vec2 vTexCoord;

end

begin uniforms

    sampler2D diffuseTexture0;

end

begin vertex_outputs

    vec2 vTex;

end

begin vertex_shader

  vTex = vTexCoord;
	VertexOut = vec4(vVertex, 0.0, 1.0);

end

begin fragment_shader

    ColorOut = texture(diffuseTexture0, vTex);

end
