-- Dungeon 5 1F

sol.main.include("maps/prison_guard")

function event_map_started(destination_point_name)

  -- torches door
  if sol.game.savegame_get_boolean(514) then
    lock_torches()
  end

  -- middle door
  if sol.game.savegame_get_boolean(522) then
    sol.map.switch_set_activated("c_door_switch", true)
  end

  -- block falled from 2F
  if not sol.game.savegame_get_boolean(515) then
    sol.map.block_set_enabled("block_from_2f", false)
  end
end

-- Returns whether all torches are on
function are_all_torches_on()

  return sol.map.interactive_entity_exists("torch_1")
      and sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch_1")) == "lit"
      and sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch_2")) == "lit"
      and sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch_3")) == "lit"
      and sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch_4")) == "lit" 
end

-- Makes all torches on forever
function lock_torches()
  sol.map.interactive_entity_remove("torch_1")
  sol.map.interactive_entity_remove("torch_2")
  sol.map.interactive_entity_remove("torch_3")
  sol.map.interactive_entity_remove("torch_4")
end

function event_map_opening_transition_finished(destination_point_name)

  -- show the welcome message
  if destination_point_name == "from_outside" then
    sol.map.dialog_start("dungeon_5.welcome")
  end
end

function event_enemy_dead(enemy_name)

  if enemy_name == "se_room_enemy"
      and not sol.map.door_is_open("se_door") then
    sol.main.play_sound("secret")
    sol.map.door_open("se_door")
  elseif string.find(enemy_name, "^s_room_enemy")
      and sol.map.enemy_is_group_dead("s_room_enemy")
      and not sol.map.door_is_open("se_door") then
    sol.main.play_sound("secret")
    sol.map.door_open("se_door")
  end
end

function event_update()

  if not sol.game.savegame_get_boolean(514)
      and are_all_torches_on() then
    sol.main.play_sound("secret")
    sol.map.door_open("w_door")
    lock_torches()
  end
end

function event_switch_activated(switch_name)

  if switch_name == "c_door_switch"
      and not sol.map.door_is_open("c_door") then
    sol.map.camera_move(504, 504, 250, open_c_door)
  elseif switch_name == "e_door_switch"
      and not sol.map.door_is_open("e_door") then
    sol.map.camera_move(1048, 488, 250, open_e_door)
  end
end

function open_c_door()
  
  sol.main.play_sound("secret")
  sol.map.door_open("c_door")
end

function open_e_door()
  
  sol.main.play_sound("secret")
  sol.map.door_open("e_door")
end

function event_hero_on_sensor(sensor_name)

  sensor_check_guard(sensor_name)
end

function event_hero_still_on_sensor(sensor_name)

  sensor_check_guard(sensor_name)
end

function event_dialog_finished(first_message_id, answer)

  dialog_check_guard(first_message_id)
end

