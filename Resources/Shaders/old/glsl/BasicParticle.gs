#extension GL_EXT_geometry_shader4 : enable

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec4 Vel[];

void main(void)
{ 
	int i;
	
	//grow a quad (billboard) around each vertex (particle)
	//note that the vertices are already multiplied by MVP so we are working in the camera clip space.
	//we want the billboards to be always facing the camera so this is actually very helpful in this case,
	//as we can simply sum the deltas.
	for (i = 0; i < gl_in.length(); i++) //we know length is 1, we could do away with the for loop.
	{
		vec4 center = gl_in[i].gl_Position;
		//common z;
		gl_Position = center;
		//bottom left
		gl_Position.x = center.x - 0.05;
		gl_Position.y = center.y - 0.05;
		EmitVertex();
		//top-left
		gl_Position.x = center.x - 0.05;
		gl_Position.y = center.y + 0.05;
		EmitVertex();			
		//bottom right
		gl_Position.x = center.x + 0.05;
		gl_Position.y = center.y - 0.05;
		EmitVertex();
		//top right
		gl_Position.x = center.x + 0.05;
		gl_Position.y = center.y + 0.05;
		EmitVertex();			
		
		EndPrimitive();
	}
	
}
