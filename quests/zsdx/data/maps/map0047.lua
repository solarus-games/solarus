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
end

function event_map_opening_transition_finished(destination_point_name)
	sol.map.dialog_start("dunegon_8.welcome")
end

function event_switch_enabled(switch_name)
	-- LB2 room
	if switch_name == "BB2" then
		sol.map.interactive_entity_remove("LB2")
		sol.main.play_sound("secret")
	end
end

function event_hero_on_sensor(sensor_name)
	-- LD1 room: when Link enters this room, door LD1 closes and enemies appear, sensor DS1 is disabled
	if sensor_name == "DS1" then
		sol.map.door_close("LD1")
		sol.map.sensor_set_enabled("DS1", false)
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
