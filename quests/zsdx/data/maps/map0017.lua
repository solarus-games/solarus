local map = ...
-- Dungeon 10 1F

function map:on_started(destination_point_name)

  map:set_light(1)

  if map:get_game():get_boolean(201) then
    lock_torches()
  end

  if map:get_game():get_boolean(228) then
    map:block_set_enabled("block_13", false)
  else
    map:block_set_enabled("block_saved", false)
  end

  if destination_point_name ~= "main_entrance" then
    map:set_doors_open("eye_door", true)
  end
end

function map:on_door_open(door_name)

  if door_name == "weak_door" then
    sol.audio.play_sound("secret")		
  end
end

function map:on_opening_transition_finished(destination_point_name)

  if destination_point_name == "main_entrance" then
    map:start_dialog("dungeon_10.welcome")
  end
end

function map:on_enemy_dead(enemy_name)

  if enemy_name:find("enemy_group1")
      and not map:has_entities("enemy_group1")
      and not map:get_game():get_boolean(200) then
    map:move_camera(616, 552, 250, function()
      map:create_pickable("small_key", 1, 200, 616, 557, 1)
      sol.audio.play_sound("secret")
    end)
  end
end

function map:on_switch_activated(switch_name)

  -- center
  if switch_name == "eye_switch"
      and not map:door_is_open("eye_door")  then
    sol.audio.play_sound("secret")
    map:open_doors("eye_door")
  end
end

function are_all_torches_on()

  return map:npc_exists("torch1")
    and map:npc_get_sprite("torch1"):get_animation() == "lit"
    and map:npc_get_sprite("torch2"):get_animation() == "lit"
    and map:npc_get_sprite("torch3"):get_animation() == "lit"
    and map:npc_get_sprite("torch4"):get_animation() == "lit"
end

function lock_torches()

  map:npc_remove("torch1")
  map:npc_remove("torch2")
  map:npc_remove("torch3")
  map:npc_remove("torch4")
end

function map:on_update()

  if not map:get_game():get_boolean(201) and are_all_torches_on() then
    map:get_game():set_boolean(201, true)
    lock_torches()
    map:move_camera(232, 488, 250, function()
      sol.audio.play_sound("secret")
      map:create_pickable("small_key", 1, 202, 232, 493, 0)
    end)
  end
end

function map:on_block_moved(block_name)

  if block_name == "block_13" then
    map:get_game():set_boolean(228, true)
  end
end

