struct particle
{
	float x;
	float y;
	float z;
	float vx;
	float vy;
	float vz;
	float life;
};

__kernel void gl_particle_rain(__global struct particle* particles, __global struct particle* state0, const float deltaT, const int num)
{
	//returns the ID of the thread in execution
	const int idx = get_global_id(0);

	//define gravity, following GL coordinate system
	float4 gravity = {0.0f, -1.1f, 0.0f, 0.0f}; //float3 is not supported, gravity value is arbitrary.
	float4 wind = {0.0f, 0.0f, 0.0f, 0.0f};
	float4 acc = gravity + wind;
	
	//each thread sums a single component.
	if (idx < num) //check that it's not oveflowing the vectors. 
	{
		struct particle p = particles[idx];
		
		if (p.life >= 0)
		{
			p.x = p.x + p.vx*deltaT + 0.5f*acc.x*(deltaT*deltaT);
			p.y = p.y + p.vy*deltaT + 0.5f*acc.y*(deltaT*deltaT);
			p.z = p.z + p.vz*deltaT + 0.5f*acc.z*(deltaT*deltaT);
		
			p.vx = p.vx + acc.x*deltaT;
			p.vy = p.vy + acc.y*deltaT;
			p.vz = p.vz + acc.z*deltaT;
		
			p.life -= deltaT;
		}
		else
		{
			p = state0[idx];
		}
	
		particles[idx] = p;
	}
}
