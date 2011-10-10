function event_map_started(destination_point_name)

	sol.map.light_set(0)

end

function event_switch_activated(switch_name)

	if switch_name=="switch1_1" then
		sol.main.play_sound("chest_appears")
		sol.map.door_open(door1)
	end


end

function event_switch_inactivated(switch_name)

	if switch_name=="switch1_1" then
		sol.main.play_sound("chest_appears")
		sol.map.door_close(door1)
	end


end