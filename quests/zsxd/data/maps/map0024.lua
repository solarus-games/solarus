----------------------------------
-- Crazy House 3F               --
----------------------------------

function event_map_started(destination_point_name)
	sol.map.sensor_set_enabled("bowser_leave", false)
	sol.map.sensor_set_enabled("bowser_exit", false)
end

function event_hero_on_sensor(sensor_name)
	if sensor_name == "bowser_message" then
		sol.main.play_sound("sm64_bowser_message")
		sol.map.dialog_start("crazy_house.infinite_greetings")
		sol.map.sensor_set_enabled("bowser_close", true)
	end
	if sensor_name == "bowser_close" then
		sol.map.door_set_open("bowser_door", false)
		sol.main.play_sound("door_closed")
		sol.map.sensor_set_enabled("bowser_exit", true)
		sol.map.sensor_set_enabled("bowser_close", false)
	end
	if sensor_name == "bowser_exit" then
		sol.map.door_set_open("bowser_door", true)
		sol.main.play_sound("door_open")
		sol.map.sensor_set_enabled("bowser_message", false)
		sol.map.sensor_set_enabled("bowser_close", true)
		sol.map.sensor_set_enabled("bowser_exit", false)
	end
	if sensor_name == "bowser_leave" then
		sol.map.door_set_open("bowser_door", false)
		sol.main.play_sound("door_closed")
		sol.map.sensor_set_enabled("bowser_message", true)
		sol.map.sensor_set_enabled("bowser_leave", false)
	end
	
	if sensor_name == "infinite_corridor" then
		sol.map.hero_set_map(24, "infinite_start", 0)
	end
end

function event_dialog_finished(first_message_id, answer)
	if first_message_id == "crazy_house.infinite_greetings" then
		sol.map.door_set_open("bowser_door", true)
		sol.main.play_sound("door_open")
	end
end
