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
	sol.map.door_set_open("LD4", true)
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
	end
end

function BB1_camera_move()
	sol.map.camera_move(896, 1712, 150)
end

function BB1_camera_restore()
	sol.map.camera_restore()
end

function BB1_remove_barrier()
	sol.map.tile_set_enabled("LB1", false)
	sol.main.play_sound("secret")
	sol.main.timer_start(BB1_camera_restore, 1000)
end

function event_camera_reached_target()
	sol.main.timer_start(BB1_remove_barrier, 1000)
end

function event_camera_back()
	sol.map.hero_unfreeze()
end


function event_hero_on_sensor(sensor_name)
	if sensor_name == "DS1" then
		-- LD1 room: when Link enters this room, door LD1 closes and enemies appear, sensor DS1 is disabled
		sol.map.door_close("LD1")
		sol.map.sensor_set_enabled("DS1", false)
	elseif sensor_name == "DS4" then
		sol.map.door_close("LD4")
		sol.map.sensor_set_enabled("DS4", false)
	end
end

function event_enemy_dead(enemy_name)
	-- LD1 room: kill all enemies will open the doors LD1 and LD2
	if string.match(enemy_name, "^room_LD1_enemy") and sol.map.enemy_is_group_dead("room_LD1_enemy") then	
		sol.map.door_open("LD1")
		sol.map.door_open("LD2")
		sol.main.play_sound("secret")
	elseif string.match(enemy_name, "^room_LD5_enemy") and sol.map.enemy_is_group_dead("room_LD5_enemy") then
		sol.map.door_open("LD5")
		sol.main.play_sound("secret")
	end
end
