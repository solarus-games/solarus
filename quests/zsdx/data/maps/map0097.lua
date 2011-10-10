function event_map_started(destination_point_name)

  	if sol.game.savegame_get_boolean(205) then
		sol.map.block_set_enabled("block1_1",false)
   		lock_torches_group1()
	end
  	if sol.game.savegame_get_boolean(206) then
		sol.map.block_set_enabled("block2_1",false)
   		lock_torches_group2()
	end

end
function are_group1_torches_on()

  return sol.map.interactive_entity_exists("torch1_1")
      and sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch1_1")) == "lit"
      and sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch1_2")) == "lit"
      and sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch1_3")) == "lit"

end

function are_group2_torches_on()

  return sol.map.interactive_entity_exists("torch2_1")
      and sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch2_1")) == "lit"
      and sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch2_2")) == "lit"
      and sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch2_3")) == "lit"

end


function lock_torches_group1()

  sol.map.interactive_entity_remove("torch1_1")
  sol.map.interactive_entity_remove("torch1_2")
  sol.map.interactive_entity_remove("torch1_3")

end

function lock_torches_group2()

  sol.map.interactive_entity_remove("torch2_1")
  sol.map.interactive_entity_remove("torch2_2")
  sol.map.interactive_entity_remove("torch2_3")

end



function event_enemy_dead(enemy_name)

	if sol.map.enemy_is_group_dead("enemy_group2") and not sol.game.savegame_get_boolean(221) then
		sol.map.pickable_item_create("small_key", 1, "221", 328,320, 1)
		sol.main.play_sound("chest_appears")
	end

end

function explode_block1() 

	sol.map.explosion_create(536,256,1)
	sol.main.play_sound("explosion")
	sol.map.block_set_enabled("block1_1",false)
	
end

function explode_block2() 

	sol.map.explosion_create(552,256,1)
	sol.main.play_sound("explosion")
	sol.map.block_set_enabled("block2_1",false)
end

function event_update()

	if not sol.game.savegame_get_boolean(205) and are_group1_torches_on() 
	 then
		sol.game.savegame_set_boolean(205, true)
   		lock_torches_group1()
		camera = explode_block1
		sol.map.camera_move(536,256,250,explode_block1)
	end


	if not sol.game.savegame_get_boolean(206) and are_group2_torches_on() 
	 then
		sol.game.savegame_set_boolean(206, true)
   		lock_torches_group2()
		camera = explode_block2
		sol.map.camera_move(552,256,250,explode_block2)
	end
end
