begin vertex

    vec3 vVertex;
    vec4 vVelocity;

end

begin uniforms

    mat4 mMV;
    mat4 mProj;
    vec4 vDiffuseColor;

end

begin vertex_outputs

    vec4 Vel;

end

begin vertex_shader

	Vel = vVelocity;
	mat4 MVP = mProj * mMV;
	VertexOut = MVP * vec4(vVertex, 1.0); 

end

begin geometry_shader max_vertex_out=4

    int i;
    
    //grow a quad (billboard) around each vertex (particle)
    //note that the vertices are already multiplied by MVP so we are working in the camera clip space.
    //we want the billboards to be always facing the camera so this is actually very helpful in this case,
    //as we can simply sum the deltas.
    for (i = 0; i < ArrayIn.length(); i++) //we know length is 1, we could do away with the for loop.
    {
        vec4 center = ArrayIn[i].VertexOut;
        //common z;
        VertexOut = center;
        //bottom left
        VertexOut.x = center.x - 0.05;
        VertexOut.y = center.y - 0.05;
        EmitVertex();
        //top-left
        VertexOut.x = center.x - 0.05;
        VertexOut.y = center.y + 0.05;
        EmitVertex();
        //bottom right
        VertexOut.x = center.x + 0.05;
        VertexOut.y = center.y - 0.05;
        EmitVertex();
        //top right
        VertexOut.x = center.x + 0.05;
        VertexOut.y = center.y + 0.05;
        EmitVertex();			
        
        EndPrimitive();
    }
    
end

begin fragment_shader

    ColorOut = vDiffuseColor;

end
