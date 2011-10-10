-- Binbin dungeon 1F script

function event_map_started(destination_point_name)
	

	sol.map.light_set(1)

  	if sol.game.savegame_get_boolean(201) then
   		lock_torches()
	end

end

function event_door_open(door_name)

	if (door_name=="weak_door") then
		sol.main.play_sound("secret")		
	end
end

function event_map_opening_transition_finished(destination_point_name)
	
	if destination_point_name == "main_entrance" then
		sol.map.dialog_start("dungeon_10")
	end

end

function event_enemy_dead(enemy_name)

	if sol.map.enemy_is_group_dead("enemy_group2") and not sol.game.savegame_get_boolean(200) then
		sol.map.pickable_item_create("small_key", 1, "200", 608,544, 1)
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

function are_all_torches_on()

  return sol.map.interactive_entity_exists("torch1")
      and sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch1")) == "lit"
      and sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch2")) == "lit"
      and sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch3")) == "lit"
      and sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch4")) == "lit"

end

function lock_torches()

  sol.map.interactive_entity_remove("torch1")
  sol.map.interactive_entity_remove("torch2")
  sol.map.interactive_entity_remove("torch3")
  sol.map.interactive_entity_remove("torch4")

end

function event_update()

	if not sol.game.savegame_get_boolean(201) and are_all_torches_on() 
	 then
		sol.main.play_sound("chest_appears")
		sol.map.pickable_item_create("small_key", 1, "202", 232,496, 0)
		sol.game.savegame_set_boolean(201, true)
   		lock_torches()
	end
end
