-- Dungeon 8 B1

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

LD7_all_enemies_dead = false

function event_map_started(destination_point_name)
	sol.map.door_set_open("LD9", true)

	-- Link has mirror shield: no laser obstacles
	if sol.game.get_ability("shield") >= 3 then
		sol.map.obstacle_set_enabled("LO4", false)
	end

	if not sol.game.savegame_get_boolean(706) then
		sol.map.chest_set_enabled("KC03", false)
	end
	if not sol.game.savegame_get_boolean(707) then
		sol.map.chest_set_enabled("KC04", false)
	end

end

function event_hero_on_sensor(sensor_name)
	if sensor_name == "DS12" then
	-- Push block room		
		sol.main.play_sound("secret")
		sol.map.door_open("LD12")
		sol.map.sensor_set_enabled("DS12", false)
	elseif sensor_name == "DS7" then
	-- Globules monsters room		
		if sol.map.door_is_open("LD7") and not LD7_all_enemies_dead then		
			sol.map.door_close("LD7")
		end
	elseif sensor_name == "DS9" then
	-- Hard hat beetles room		
		if sol.map.door_is_open("LD9") then
			sol.map.door_close("LD9")
			sol.map.sensor_set_enabled("DS9", false)
		end
	end
end

function event_switch_activated(switch_name)
	if switch_name == "CB03" then
		if not sol.game.savegame_get_boolean(706) then
			sol.map.hero_freeze()
			sol.main.timer_start(CB03_camera_move, 750)
		end
	elseif switch_name == "CB04" then
		sol.map.chest_set_enabled("KC04", true)
		sol.main.play_sound("chest_appears")
	elseif string.match(switch_name, "^DB08") then
		if not sol.map.door_is_open("LD8") then
			sol.map.door_open("LD8")
		end
		if not sol.map.door_is_open("LD7") then
			sol.map.door_open("LD7")
		end
	end
end

function CB03_camera_move()
	sol.map.camera_move(1488, 1152, 250, CB03_chest_appears)
end

function CB03_chest_appears()
	sol.map.chest_set_enabled("KC03", true)
	sol.main.play_sound("chest_appears")
end

function CB03_time_out()
	sol.map.chest_set_enabled("KC03", false)
	sol.map.switch_set_activated("CB03", false)
end

function event_camera_back()
	sol.map.hero_unfreeze()
	sol.main.timer_start(CB03_time_out, 10000, true)
end

function event_enemy_dead(enemy_name)
	if string.match(enemy_name, "^LD7_enemy") and sol.map.enemy_is_group_dead("LD7_enemy") then	
	-- LD7 room: kill all enemies will open the door LD7
		sol.map.door_open("LD7")
		sol.main.play_sound("secret")
		LD7_all_enemies_dead = true
	elseif string.match(enemy_name, "^LD9_enemy") and sol.map.enemy_is_group_dead("LD9_enemy") then	
	-- LD7 room: kill all enemies will open the door LD7
		sol.map.door_open("LD9")
		sol.main.play_sound("secret")
	end
end

function event_door_open(door_name)

  if door_name == "WW02" then
    sol.main.play_sound("secret")
  end
end

