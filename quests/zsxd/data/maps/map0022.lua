----------------------------------
-- Crazy House 2F (south)       --
----------------------------------

locked_door_A_value = 0

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

function event_switch_activated(switch_name)
	if switch_name == "locked_door_switch_A"
	or switch_name == "locked_door_switch_B" then
		if locked_door_A_value < 2 then
			locked_door_A_value = locked_door_A_value + 1
			if locked_door_A_value == 2 then
				sol.map.door_set_open("locked_door_A", true)
				sol.main.play_sound("door_open")
			end
		end
	end
end
