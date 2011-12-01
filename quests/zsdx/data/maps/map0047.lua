-- Dungeon 8 1F

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

fighting_boss = false

function event_map_started(destination_point_name)
	sol.map.door_set_open("LD1", true)
	sol.map.door_set_open("LD3", true)
	sol.map.door_set_open("LD4", true)
	sol.map.door_set_open("boss_door", true)
	sol.map.npc_set_enabled("billy_npc", false)

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

	if destination_point_name == "from_boss" or destination_point_name == "from_hidden_room" then
		sol.map.door_set_open("LD5", true)
	end
end

function event_map_opening_transition_finished(destination_point_name)
	if destination_point_name == "from_outside" then	
		sol.map.dialog_start("dungeon_8.welcome")
	end
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
	elseif sensor_name == "start_boss_sensor" then
		if not fighting_boss and not sol.game.savegame_get_boolean(727) then
			sol.map.door_close("boss_door")
			sol.map.npc_set_enabled("billy_npc", true)
			sol.map.hero_freeze()
			fighting_boss = true
			sol.main.timer_start(function()
				sol.map.dialog_start("dungeon_8.billy")
			end, 1000)
		end
	end
end

function event_enemy_dead(enemy_name)
	if string.match(enemy_name, "^room_LD1_enemy") and sol.map.enemy_is_group_dead("room_LD1_enemy") then	
	-- LD1 room: kill all enemies will open the doors LD1 and LD2
		sol.map.door_open("LD1")
		sol.map.door_open("LD2")
		sol.main.play_sound("secret")
	elseif string.match(enemy_name, "^room_LD5_enemy") and sol.map.enemy_is_group_dead("room_LD5_enemy") and not sol.map.door_is_open("LD5") then
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
	elseif enemy_name == "boss" then
		sol.main.timer_start(function()
			sol.main.play_music("victory.spc")
			sol.map.hero_freeze()
			sol.map.hero_set_direction(3)
		end, 1000)
		sol.main.timer_start(function()
			sol.map.door_open("boss_door")
			sol.main.play_sound("secret")
			sol.map.hero_unfreeze()
		end, 10000)
	end
end

function event_dialog_finished(first_message_id)

	if first_message_id == "dungeon_8.billy" then
		sol.main.play_music("boss.spc")
		sol.map.hero_unfreeze()
		sol.map.enemy_set_enabled("boss", true)
		sol.map.npc_set_enabled("billy_npc", false)
	end
end

