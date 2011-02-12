----------------------------------
-- Crazy House B1 (south)       --
----------------------------------

function event_map_started(destination_point_name)
	sol.map.chest_set_enabled("prankster_chest_bottom", false)
	sol.map.chest_set_enabled("prankster_chest_middle", false)
end

function event_hero_on_sensor(sensor_name)
	if sensor_name == "prankster_sensor_top" then
		sol.map.chest_set_enabled("prankster_chest_bottom", false)
		sol.map.chest_set_enabled("prankster_chest_middle", true)
	elseif sensor_name == "prankster_sensor_middle" then
		sol.map.chest_set_enabled("prankster_chest_bottom", true)
		sol.map.chest_set_enabled("prankster_chest_middle", false)	
	elseif sensor_name == "prankster_sensor_bottom" then
		sol.map.chest_set_enabled("prankster_chest_bottom", false)
		sol.map.chest_set_enabled("prankster_chest_middle", true)
	end
end
