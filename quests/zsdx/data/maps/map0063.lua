-- Dungeon 5 2F

sol.main.include("maps/prison_guard")

function event_map_started(destination_point_name)

  init_guard("guard_3", "666666666666666666644444222222222222222222200000")

  if sol.game.savegame_get_boolean(515) then
    sol.map.tile_set_enabled("weak_floor", false)
  end
end

function event_map_opening_transition_finished(destination_point_name)

  -- show the welcome message
  if destination_point_name == "from_roof"
      or destination_point_name == "from_outside_w"
      or destination_point_name == "from_outside_e" then
    sol.map.dialog_start("dungeon_5.welcome")
  end
end

function init_guard(guard_name, path)

  local sprite = sol.map.npc_get_sprite(guard_name)
  local m = sol.main.path_movement_create(path, 48)
  sol.main.movement_set_property(m, "loop", true)
  sol.main.movement_set_property(m, "ignore_obstacles", true)
  sol.map.npc_start_movement(guard_name, m)
  sol.main.sprite_set_animation(sprite, "walking")
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

function event_switch_activated(switch_name)

  if switch_name == "statue_switch"
      and not sol.map.door_is_open("statue_door") then
    camera_timer = "open_statue_door"
    sol.map.camera_move(432, 536, 250)
  end
end

function event_camera_reached_target()

  sol.main.timer_start(1000, camera_timer)
end

function open_statue_door()

  sol.main.play_sound("secret")
  sol.main.play_sound("door_open")
  sol.map.door_open("statue_door")
  sol.main.timer_start(1000, "sol.map.camera_restore")
end

-- Returns whether all torches are on
function are_all_torches_on()

  return sol.map.interactive_entity_exists("torch_1")
      and sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch_1")) == "lit"
      and sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch_2")) == "lit"
      and sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch_3")) == "lit"
end

-- Makes all torches on forever
function lock_torches()
  sol.map.interactive_entity_remove("torch_1")
  sol.map.interactive_entity_remove("torch_2")
  sol.map.interactive_entity_remove("torch_3")
end

function event_update()

  if not sol.map.door_is_open("torches_door")
      and are_all_torches_on() then
    sol.main.play_sound("secret")
    sol.map.door_open("torches_door")
    lock_torches()
  end
end

function event_sensor_collision_explosion(sensor_name)

  if sensor_name == "weak_floor_sensor"
      and sol.map.tile_is_enabled("weak_floor") then

    sol.map.tile_set_enabled("weak_floor", false)
    sol.main.play_sound("secret")
    sol.game.savegame_set_boolean(515, true)
    sol.main.timer_start(1500, "weak_floor_block_fall")
  end
end

function weak_floor_block_fall()

  sol.map.block_set_enabled("weak_floor_block", false)
  sol.main.play_sound("jump")
  sol.main.timer_start(200, "weak_floor_block_fall_end")
end

function weak_floor_block_fall_end()

  sol.main.play_sound("bomb")
end

