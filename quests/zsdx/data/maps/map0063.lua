-- Dungeon 5 2F

sol.main.include("maps/prison_guard")

puzzle_next_sensor = 1

function event_map_started(destination_point_name)

  init_guard("guard_3", {6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,4,4,4,4,4,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0})

  if sol.map.get_game():get_boolean(515) then
    sol.map.tile_set_enabled("weak_floor", false)
    sol.map.sensor_set_enabled("weak_floor_sensor", false)
    sol.map.block_set_enabled("weak_floor_block", false)
  end

  -- blocks necessary to go back when found the feather
  if sol.map.get_game():get_boolean(517) then
    sol.map.block_set_position("s_block_1", 464, 773)
    sol.map.block_set_position("s_block_2", 496, 773)
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
  local m = sol.movement.create("path")
  m:set_path(path)
  m:set_speed(48)
  m:set_loop(true)
  m:set_ignore_obstacles(true)
  sol.map.npc_start_movement(guard_name, m)
  sprite:set_animation("walking")
end

function event_hero_on_sensor(sensor_name)

  sensor_check_guard(sensor_name)

  if sensor_name == "puzzle_wrong_sensor" then
    puzzle_next_sensor = 1
  elseif sensor_name == "save_s_blocks_sensor" then
    if sol.map.get_game():has_item("feather")
        and not sol.map.get_game():get_boolean(517) then
      -- solved the blocks puzzle necessary to exit this floor
      sol.map.get_game():set_boolean(517, true)
    end
  else
    local i = string.match(sensor_name, "puzzle_sensor_([1-4])")
    if i ~= nil
        and not sol.map.get_game():get_boolean(519) then
      i = tonumber(i)
      if puzzle_next_sensor == 5 and i == 1 then
        puzzle_solved()
      elseif i == puzzle_next_sensor then
	puzzle_next_sensor = puzzle_next_sensor + 1
      else
	puzzle_next_sensor = 1
      end
    end
  end
end

function event_hero_still_on_sensor(sensor_name)

  sensor_check_guard(sensor_name)
end

function event_dialog_finished(dialog_id, answer)

  dialog_check_guard(dialog_id)

  if dialog_id == "dungeon_5.puzzle_solved" then
    sol.map.hero_unfreeze()
  end
end

function event_switch_activated(switch_name)

  if switch_name == "statue_switch"
      and not sol.map.door_is_open("statue_door") then
    sol.map.camera_move(432, 536, 250, open_statue_door)
  end
end

function open_statue_door()

  sol.audio.play_sound("secret")
  sol.audio.play_sound("door_open")
  sol.map.door_open("statue_door")
end

-- Returns whether all torches are on
function are_all_torches_on()

  return sol.map.npc_exists("torch_1")
      and sol.map.npc_get_sprite("torch_1"):get_animation() == "lit"
      and sol.map.npc_get_sprite("torch_2"):get_animation() == "lit"
      and sol.map.npc_get_sprite("torch_3"):get_animation() == "lit"
end

-- Makes all torches on forever
function lock_torches()
  sol.map.npc_remove("torch_1")
  sol.map.npc_remove("torch_2")
  sol.map.npc_remove("torch_3")
end

function event_update()

  if not sol.map.door_is_open("torches_door")
      and are_all_torches_on() then

    sol.audio.play_sound("secret")
    sol.map.door_open("torches_door")
    lock_torches()
  end
end

function event_sensor_collision_explosion(sensor_name)

  if sensor_name == "weak_floor_sensor"
      and sol.map.tile_is_enabled("weak_floor") then

    sol.map.tile_set_enabled("weak_floor", false)
    sol.map.sensor_set_enabled("weak_floor_sensor", false)
    sol.audio.play_sound("secret")
    sol.map.get_game():set_boolean(515, true)
    sol.main:start_timer(1500, weak_floor_block_fall)
  end
end

function weak_floor_block_fall()

  sol.map.block_set_enabled("weak_floor_block", false)
  sol.audio.play_sound("jump")
  sol.main:start_timer(200, weak_floor_block_fall_end)
end

function weak_floor_block_fall_end()

  sol.audio.play_sound("bomb")
end

function puzzle_solved()

  sol.map.hero_freeze()
  sol.audio.play_sound("enemy_awake")
  sol.main:start_timer(1000, puzzle_solved_2)
end

function puzzle_solved_2()

  sol.main:start_timer(1000, puzzle_solved_3)
end

function puzzle_solved_3()

  sol.audio.play_sound("secret")
  sol.map.get_game():set_boolean(519, true)
  sol.map.dialog_start("dungeon_5.puzzle_solved")
end

