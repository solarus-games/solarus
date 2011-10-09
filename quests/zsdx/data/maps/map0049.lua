-----------------------------
-- Dungeon 8 B2 script     --
-----------------------------

-- Legend
-- RC: Rupee Chest
-- KC: Key Chest
-- KP: Key Pot
-- LD: Locked Door
-- KD: Key Door
-- DB: Door Button
-- LB: Locked Barrier
-- BB: Barrier Button
-- DS: Door Sensor

function event_map_started(destination_point_name)
	sol.map.door_set_open("LD11", true)
end

function button_room_test_combination()
	if sol.map.switch_is_activated("BRoom1")
	and sol.map.switch_is_activated("BRoom2")
	and sol.map.switch_is_activated("BRoom3")
	and sol.map.switch_is_activated("BRoom4")
	and sol.map.switch_is_activated("BRoom5")
	and sol.map.switch_is_activated("BRoom6")
	and sol.map.switch_is_activated("BRoom7")
	and sol.map.switch_is_activated("BRoom8")
	and sol.map.switch_is_activated("BRoom9") then
		if not sol.map.door_is_open("LD15") then
			sol.main.play_sound("secret")
			sol.map.door_open("LD15")
		end
	end
end

function event_switch_activated(switch_name)
	if switch_name == "BRoom1" then
		sol.map.switch_set_activated("BRoom2", not sol.map.switch_is_activated("BRoom2"))
		sol.map.switch_set_activated("BRoom4", not sol.map.switch_is_activated("BRoom4"))
		sol.map.switch_set_activated("BRoomReset", false)
		button_room_test_combination()
	elseif switch_name == "BRoom2" then
		sol.map.switch_set_activated("BRoom1", not sol.map.switch_is_activated("BRoom1"))
		sol.map.switch_set_activated("BRoom3", not sol.map.switch_is_activated("BRoom3"))
		sol.map.switch_set_activated("BRoom5", not sol.map.switch_is_activated("BRoom5"))
		sol.map.switch_set_activated("BRoomReset", false)
		button_room_test_combination()
	elseif switch_name == "BRoom3" then
		sol.map.switch_set_activated("BRoom2", not sol.map.switch_is_activated("BRoom2"))
		sol.map.switch_set_activated("BRoom6", not sol.map.switch_is_activated("BRoom6"))
		sol.map.switch_set_activated("BRoomReset", false)
		button_room_test_combination()
	elseif switch_name == "BRoom4" then
		sol.map.switch_set_activated("BRoom1", not sol.map.switch_is_activated("BRoom1"))
		sol.map.switch_set_activated("BRoom5", not sol.map.switch_is_activated("BRoom5"))
		sol.map.switch_set_activated("BRoom7", not sol.map.switch_is_activated("BRoom7"))
		sol.map.switch_set_activated("BRoomReset", false)
		button_room_test_combination()
	elseif switch_name == "BRoom5" then
		sol.map.switch_set_activated("BRoom2", not sol.map.switch_is_activated("BRoom2"))
		sol.map.switch_set_activated("BRoom4", not sol.map.switch_is_activated("BRoom4"))
		sol.map.switch_set_activated("BRoom6", not sol.map.switch_is_activated("BRoom6"))
		sol.map.switch_set_activated("BRoom8", not sol.map.switch_is_activated("BRoom8"))
		sol.map.switch_set_activated("BRoomReset", false)
		button_room_test_combination()
	elseif switch_name == "BRoom6" then
		sol.map.switch_set_activated("BRoom3", not sol.map.switch_is_activated("BRoom3"))
		sol.map.switch_set_activated("BRoom5", not sol.map.switch_is_activated("BRoom5"))
		sol.map.switch_set_activated("BRoom9", not sol.map.switch_is_activated("BRoom9"))
		sol.map.switch_set_activated("BRoomReset", false)
		button_room_test_combination()
	elseif switch_name == "BRoom7" then
		sol.map.switch_set_activated("BRoom4", not sol.map.switch_is_activated("BRoom4"))
		sol.map.switch_set_activated("BRoom8", not sol.map.switch_is_activated("BRoom8"))
		sol.map.switch_set_activated("BRoomReset", false)
		button_room_test_combination()
	elseif switch_name == "BRoom8" then
		sol.map.switch_set_activated("BRoom5", not sol.map.switch_is_activated("BRoom5"))
		sol.map.switch_set_activated("BRoom7", not sol.map.switch_is_activated("BRoom7"))
		sol.map.switch_set_activated("BRoom9", not sol.map.switch_is_activated("BRoom9"))
		sol.map.switch_set_activated("BRoomReset", false)
		button_room_test_combination()
	elseif switch_name == "BRoom9" then
		sol.map.switch_set_activated("BRoom6", not sol.map.switch_is_activated("BRoom6"))
		sol.map.switch_set_activated("BRoom8", not sol.map.switch_is_activated("BRoom8"))
		sol.map.switch_set_activated("BRoomReset", false)
		button_room_test_combination()
	elseif switch_name == "BRoomReset" then
		sol.map.switch_set_activated("BRoom1", false)
		sol.map.switch_set_activated("BRoom2", false)
		sol.map.switch_set_activated("BRoom3", false)
		sol.map.switch_set_activated("BRoom4", false)
		sol.map.switch_set_activated("BRoom5", false)
		sol.map.switch_set_activated("BRoom6", false)
		sol.map.switch_set_activated("BRoom7", false)
		sol.map.switch_set_activated("BRoom8", false)
		sol.map.switch_set_activated("BRoom9", false)
	elseif switch_name == "DB16" then
		sol.map.hero_freeze()
		sol.main.timer_start(DB16_camera_move, 750)
	end
end

function DB16_camera_move()
	sol.map.camera_move(808, 1056, 250)
end

function DB16_camera_restore()
	sol.map.camera_restore()
end

function DB16_open_door()
	sol.map.door_open("LD16")
	sol.main.timer_start(DB16_camera_restore, 500)
end

function DB16_time_out()
	sol.map.door_close("LD16")
	sol.map.switch_set_activated("DB16", false)
end

function event_camera_reached_target()
	sol.main.timer_start(DB16_open_door, 500)
end

function event_camera_back()
	sol.map.hero_unfreeze()
	sol.main.timer_start(DB16_time_out, 10000, true)
end

function event_hero_on_sensor(sensor_name)
	if sensor_name == "DS11" then
		sol.map.door_close("LD11")
		sol.map.sensor_set_enabled("DS11", false)
	end
end
