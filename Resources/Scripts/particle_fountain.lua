require "math"

function GenerateParticleCloud(max_particles)

	local pos_offset = 0
	local vel_offset = 3
	local life_offset = 6
	local size = 7
	
	local cloud = {}
	
	math.randomseed(5)
	--math.random() returns values form 0 to 1	
	for i = 0, max_particles do
		
		cloud[i*size + pos_offset + 0] = math.random()*0.5 - 0.25
		cloud[i*size + pos_offset + 1] = -5.0
		cloud[i*size + pos_offset + 2] = math.random()*0.5 - 0.25
		
		cloud[i*size + vel_offset + 0] = math.random()*0.4 - 0.2
		cloud[i*size + vel_offset + 1] = math.random()*3.5 + 1.2
		cloud[i*size + vel_offset + 2] = math.random()*0.4 - 0.2

		cloud[i*size + life_offset] = math.random()*10.0 
		
	end

	return cloud
	
end
