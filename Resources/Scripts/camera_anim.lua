require "math"

_G.angle = 1.57

function MoveCamera(pos_in, dt)

    -- circular motion around y

    local position = {}

	local r = math.sqrt(pos_in[0]*pos_in[0] + pos_in[2]*pos_in[2])

    -- 0.05 rad/sec  ~3 deg/sec
    local ang_vel = 0.05

    if pos_in[0] == 0 then
        pos_in[0] = 1e-3
    end

    -- local angle = math.atan(pos_in[2], pos_in[0])
    _G.angle = _G.angle + ang_vel*dt

	-- x
    position[0] = r*math.sin(_G.angle)
	-- constant y
    position[1] = pos_in[1]
	-- z
    position[2] = r*math.cos(_G.angle)
	
	return position
	
end
