local map = ...
-- Dungeon 5 1F

sol.main.do_file("maps/prison_guard")

function map:on_started(destination_point_name)

  -- torches door
  if map:get_game():get_boolean(514) then
    lock_torches()
  end

  -- middle door
  if map:get_game():get_boolean(522) then
    map:switch_set_activated("c_door_switch", true)
  end

  -- block falled from 2F
  if not map:get_game():get_boolean(515) then
    map:block_set_enabled("block_from_2f", false)
  end
end

-- Returns whether all torches are on
function are_all_torches_on()

  return map:npc_exists("torch_1")
      and map:npc_get_sprite("torch_1"):get_animation() == "lit"
      and map:npc_get_sprite("torch_2"):get_animation() == "lit"
      and map:npc_get_sprite("torch_3"):get_animation() == "lit"
      and map:npc_get_sprite("torch_4"):get_animation() == "lit" 
end

-- Makes all torches on forever
function lock_torches()
  map:npc_remove("torch_1")
  map:npc_remove("torch_2")
  map:npc_remove("torch_3")
  map:npc_remove("torch_4")
end

function map:on_map_opening_transition_finished(destination_point_name)

  -- show the welcome message
  if destination_point_name == "from_outside" then
    map:start_dialog("dungeon_5.welcome")
  end
end

function map:on_enemy_dead(enemy_name)

  if enemy_name == "se_room_enemy"
      and not map:door_is_open("se_door") then
    sol.audio.play_sound("secret")
    map:open_doors("se_door")
  elseif string.find(enemy_name, "^s_room_enemy")
      and map:enemy_is_group_dead("s_room_enemy")
      and not map:door_is_open("se_door") then
    sol.audio.play_sound("secret")
    map:open_doors("se_door")
  end
end

function map:on_update()

  if not map:get_game():get_boolean(514)
      and are_all_torches_on() then
    sol.audio.play_sound("secret")
    map:open_doors("w_door")
    lock_torches()
  end
end

function map:on_switch_activated(switch_name)

  if switch_name == "c_door_switch"
      and not map:door_is_open("c_door") then
    map:move_camera(504, 504, 250, open_c_door)
  elseif switch_name == "e_door_switch"
      and not map:door_is_open("e_door") then
    map:move_camera(1048, 488, 250, open_e_door)
  end
end

function open_c_door()
  
  sol.audio.play_sound("secret")
  map:open_doors("c_door")
end

function open_e_door()
  
  sol.audio.play_sound("secret")
  map:open_doors("e_door")
end

function map:on_hero_on_sensor(sensor_name)

  sensor_check_guard(sensor_name)
end

function map:on_hero_still_on_sensor(sensor_name)

  sensor_check_guard(sensor_name)
end

function map:on_dialog_finished(dialog_id, answer)

  dialog_check_guard(dialog_id)
end

