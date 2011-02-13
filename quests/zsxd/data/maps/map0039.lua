----------------------------------
-- FREAKING CAVE 2 OMFG	        --
----------------------------------

	

function event_map_started(destination_point_name)
	
	-- No light inside the cave at start
	sol.map.light_set(0) 	
	-- Let the trap door openned
	sol.map.door_open("trap_door")
end

function event_hero_on_sensor(sensor_name)
	
	-- Closing trap door	
	if sensor_name == "sensor_close_trap_door" then
		sol.map.door_close("trap_door")
		sol.map.sensor_set_enabled("sensor_close_trap_door",false)
	end
end
