-----------------------------
-- Dungeon 8 1F script     --
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
	sol.map.door_set_open("LD1", true)
	sol.map.door_set_open("LD3", true)
	sol.map.door_set_open("LD4", true)

	-- Map chest hide if not already opened
	if not sol.game.savegame_get_boolean(700) then
		sol.map.chest_set_enabled("MAP", false)
	end

	-- Big key chest hide if not already opened
	if not sol.game.savegame_get_boolean(705) then
		sol.map.chest_set_enabled("BK01", false)
	end

	-- Link has mirror shield: no laser obstacles
  if sol.game.get_ability("shield") >= 3 then
		sol.map.obstacle_set_enabled("LO1", false)
		sol.map.obstacle_set_enabled("LO2", false)
	end
end

function event_map_opening_transition_finished(destination_point_name)
	sol.map.dialog_start("dungeon_8.welcome")
end

function event_switch_activated(switch_name)
	if switch_name == "BB1" then
	-- LB1 room
		sol.map.hero_freeze()
		sol.main.timer_start(BB1_camera_move, 1000)
	elseif switch_name == "BB2" then
	-- LB2 room
		sol.map.tile_set_enabled("LB2", false)
		sol.main.play_sound("secret")
	elseif switch_name == "DB4" then
		sol.map.door_open("LD4")
		sol.main.play_sound("secret")
	elseif switch_name == "DB06" then
		-- 4 statues room door opening		
		sol.map.door_open("LD6")
		sol.main.play_sound("secret")
	elseif string.match(switch_name, "^RPS") then
		-- Resets position of statues
		sol.map.block_reset("STT1")
		sol.map.block_reset("STT2")
		sol.map.block_reset("STT3")
		sol.map.block_reset("STT4")
		sol.main.play_sound("warp")
	end
end

function BB1_remove_barrier()
	sol.map.tile_set_enabled("LB1", false)
	sol.main.play_sound("secret")
end

function BB1_camera_move()
	sol.map.camera_move(896, 1712, 250, BB1_remove_barrier)
end

function event_camera_back()
	sol.map.hero_unfreeze()
end


function event_hero_on_sensor(sensor_name)
	if sensor_name == "DS1" then
		-- LD1 room: when Link enters this room, door LD1 closes and enemies appear, sensor DS1 is disabled
		sol.map.door_close("LD1")
		sol.map.sensor_set_enabled("DS1", false)
	elseif sensor_name == "DS3" then
		sol.map.door_close("LD3")
		sol.map.sensor_set_enabled("DS3", false)
	elseif sensor_name == "DS4" then
		sol.map.door_close("LD4")
		sol.map.sensor_set_enabled("DS4", false)
	end
end

function event_enemy_dead(enemy_name)
	if string.match(enemy_name, "^room_LD1_enemy") and sol.map.enemy_is_group_dead("room_LD1_enemy") then	
	-- LD1 room: kill all enemies will open the doors LD1 and LD2
		sol.map.door_open("LD1")
		sol.map.door_open("LD2")
		sol.main.play_sound("secret")
	elseif string.match(enemy_name, "^room_LD5_enemy") and sol.map.enemy_is_group_dead("room_LD5_enemy") then
	-- LD5 room: kill all enemies will open the door LD5
		sol.map.door_open("LD5")
		sol.main.play_sound("secret")
	elseif string.match(enemy_name, "^map_enemy") and sol.map.enemy_is_group_dead("map_enemy") then
	-- Map chest room: kill all enemies and the chest will appear
		if not sol.game.savegame_get_boolean(700) then		
			sol.map.chest_set_enabled("MAP", true)
			sol.main.play_sound("chest_appears")
		end
		sol.map.door_open("LD3")
		sol.main.play_sound("secret")
	elseif string.match(enemy_name, "^room_big_enemy") and sol.map.enemy_is_group_dead("room_big_enemy") then
  -- Big key chest room: kill all enemies and the chest will appear
		if not sol.game.savegame_get_boolean(705) then		
			sol.map.chest_set_enabled("BK01", true)
			sol.main.play_sound("chest_appears")
		end
	end
end
