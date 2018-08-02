begin vertex

    vec3 vVertex;

end

begin uniforms

    mat4 mMVP;
    vec4 vDiffuseColor;

end

begin vertex_shader

	VertexOut = mMVP * vec4(vVertex, 1.0); 

end

begin fragment_shader

    ColorOut = vDiffuseColor;

end
