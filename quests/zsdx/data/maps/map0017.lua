-- Binbin dungeon 1F script

seroom_door_sensor_1 = true

function event_map_started(destination_point_name)
	
	-- Init blocks
	sol.map.stairs_set_enabled("so_stair_1",false)
	-- Init chest
	-- Init stairs	
	sol.map.stairs_set_enabled("so_stair_1",false)

end

function event_map_opening_transition_finished(destination_point_name)
	
	if destination_point_name == "main_entrance" then
		sol.map.dialog_start("dungeon_10")
	end

end


function event_enemy_dead(enemy_name)

	if sol.map.enemy_is_group_dead("croom_enemy") and not sol.game.savegame_get_boolean(202) then
		sol.map.pickable_item_create("small_key", 1, "202", 728,536, 1)
		sol.main.play_sound("chest_appears")
	end

end

function event_switch_enabled(switch_name)

	-- center
	if switch_name == "croom_switch_1" and not sol.map.door_is_open("seroom_door_close_1")  then
		sol.map.camera_move(752, 720,150)
	end
	-- se
	if switch_name == "seroom_switch_1" then
		sol.map.door_open("seroom_door_close")
		seroom_door_sensor_1 = false		
	end
	-- so
	if switch_name == "so_switch_stair_1" then
		sol.map.stairs_set_enabled("so_stair_1",true)
		sol.main.play_sound("secret")
	end

end

function event_hero_on_sensor(sensor_name)

	if sensor_name=="seroom_door_sensor_1" and seroom_door_sensor_1 == false then
		sol.map.door_close("seroom_door_close")
		seroom_door_sensor_1 = true
	end

	if sensor_name=="seroom_door_sensor_2" and seroom_door_sensor_1 == true then
		sol.map.door_close("seroom_door_close")
		seroom_door_sensor_1 = false
		sol.map.switch_set_enabled("seroom_switch_1", false)
	end

end

function event_camera_reached_target()

	sol.map.door_open("seroom_door_close")
	sol.map.camera_restore()

end
