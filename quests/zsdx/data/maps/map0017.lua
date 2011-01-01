-- Binbin dungeon 1F script

function event_map_started(destination_point_name)
	
	if not sol.game.savegame_get_boolean(201) then
		sol.map.chest_set_hidden("chest1",true)
	end

end

function event_map_opening_transition_finished(destination_point_name)
	
	if destination_point_name == "main_entrance" then
		sol.map.dialog_start("dungeon_10")
	end

end


function event_enemy_dead(enemy_name)

	if sol.map.enemy_is_group_dead("seroom_enemy") and sol.map.chest_is_hidden("chest1") then
		sol.map.chest_set_hidden("chest1",false)
		sol.main.play_sound("chest_appears")
	end

end

function event_switch_enabled(switch_name)

	if switch_name == "so_switch_stair_1" then
		sol.map.tile_set_group_enabled("so_stair_wall",false)
		sol.main.play_sound("secret")


	end

end
