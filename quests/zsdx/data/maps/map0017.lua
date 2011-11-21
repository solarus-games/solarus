-- Dungeon 10 1F

function event_map_started(destination_point_name)

  sol.map.light_set(1)

  if sol.game.savegame_get_boolean(201) then
    lock_torches()
  end

  if sol.game.savegame_get_boolean(228) then
    sol.map.block_set_enabled("block_13", false)
  else
    sol.map.block_set_enabled("block_saved", false)
  end
end

function event_door_open(door_name)

  if door_name == "weak_door" then
    sol.main.play_sound("secret")		
  end
end

function event_map_opening_transition_finished(destination_point_name)

  if destination_point_name == "main_entrance" then
    sol.map.dialog_start("dungeon_10.welcome")
  end
end

function event_enemy_dead(enemy_name)

  if enemy_name:find("enemy_group1")
      and sol.map.enemy_is_group_dead("enemy_group1")
      and not sol.game.savegame_get_boolean(200) then
    sol.map.camera_move(616, 552, 250, function()
      sol.map.pickable_item_create("small_key", 1, 200, 616, 557, 1)
      sol.main.play_sound("secret")
    end)
  end
end

function event_switch_activated(switch_name)

  -- center
  if switch_name == "eye_switch"
      and not sol.map.door_is_open("eye_door")  then
    sol.main.play_sound("secret")
    sol.map.door_open("eye_door")
  end
end

function are_all_torches_on()

  return sol.map.npc_exists("torch1")
    and sol.main.sprite_get_animation(sol.map.npc_get_sprite("torch1")) == "lit"
    and sol.main.sprite_get_animation(sol.map.npc_get_sprite("torch2")) == "lit"
    and sol.main.sprite_get_animation(sol.map.npc_get_sprite("torch3")) == "lit"
    and sol.main.sprite_get_animation(sol.map.npc_get_sprite("torch4")) == "lit"
end

function lock_torches()

  sol.map.npc_remove("torch1")
  sol.map.npc_remove("torch2")
  sol.map.npc_remove("torch3")
  sol.map.npc_remove("torch4")
end

function event_update()

  if not sol.game.savegame_get_boolean(201) and are_all_torches_on() then
    sol.game.savegame_set_boolean(201, true)
    lock_torches()
    sol.map.camera_move(232, 488, 250, function()
      sol.main.play_sound("secret")
      sol.map.pickable_item_create("small_key", 1, 202, 232, 493, 0)
    end)
  end
end

function event_block_moved(block_name)

  if block_name == "block_13" then
    sol.game.savegame_set_boolean(228, true)
  end
end

