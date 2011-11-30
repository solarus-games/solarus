-----------------------------
-- Dungeon 8 B3 script     --
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
	sol.map.door_set_open("LD12", true)
	if sol.game.savegame_get_boolean(725) then
		sol.map.block_set_enabled("STT5", false)
	end
	if sol.game.savegame_get_boolean(720) then
		sol.map.switch_set_activated("DB1", true)
	end
	if sol.game.savegame_get_boolean(721) then
		sol.map.switch_set_activated("DB2", true)
	end
	if sol.game.savegame_get_boolean(720) and sol.game.savegame_get_boolean(721) then
		sol.map.door_set_open("LD14", true)
	end
end

function event_hero_on_sensor(sensor_name)
	if sensor_name == "DS12" then
		sol.map.door_close("LD12")
		sol.map.sensor_set_enabled("DS12", false)
	end
end

function event_block_moved(block_name)
	x, y = sol.map.block_get_position("STT5")
	if x >= 1088 and x <= 1168 then
		sol.map.block_set_enabled("STT5", false)
		sol.main.play_sound("jump")
		sol.game.savegame_set_boolean(725, true)
	end
end

function event_switch_activated(switch_name)
	if switch_name == "DB1" then
		sol.game.savegame_set_boolean(720, true)
		if sol.game.savegame_get_boolean(721) then
			sol.map.door_open("LD14")
			sol.main.play_sound("secret")
		end
	elseif switch_name == "DB2" then
		sol.game.savegame_set_boolean(721, true)
		if sol.game.savegame_get_boolean(720) then
			sol.map.door_open("LD14")
			sol.main.play_sound("secret")
		end
	elseif switch_name == "DB3" then
		sol.map.door_open("LD12")
		sol.map.door_open("LD13")
		sol.main.play_sound("secret")
	end
	if DB1_status == true and DB2_status == true then
		sol.map.door_open("LD14")
		sol.main.play_sound("secret")
	end
end
