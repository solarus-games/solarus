-- Binbin dungeon 1F script

function event_map_started(destination_point_name)
	
	-- Init blocks
	-- Init chest
	-- Init stairs	

end

function event_map_opening_transition_finished(destination_point_name)
	
	if destination_point_name == "main_entrance" then
		sol.map.dialog_start("dungeon_10")
	end

end


function event_enemy_dead(enemy_name)

	if sol.map.enemy_is_group_dead("enemy_group1") and not sol.game.savegame_get_boolean(202) then
		sol.map.pickable_item_create("small_key", 1, "202", 608,544, 1)
		sol.main.play_sound("chest_appears")
	end

end

function event_switch_activated(switch_name)

	-- center
	if switch_name == "switch_eye1_1" and not sol.map.door_is_open("door_eye1_1")  then
		sol.map.door_open("door_eye1_1");
		sol.map.door_open("door_eye1_2");
	end

end

function event_hero_on_sensor(sensor_name)



end

function event_camera_reached_target()

	sol.map.door_open("seroom_door_close")
	sol.map.camera_restore()

end
