local map = ...
-- Dungeon 5 2F

sol.main.do_file("maps/prison_guard")

puzzle_next_sensor = 1

function map:on_started(destination_point_name)

  init_guard("guard_3", {6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,4,4,4,4,4,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0})

  if map:get_game():get_boolean(515) then
    map:tile_set_enabled("weak_floor", false)
    map:sensor_set_enabled("weak_floor_sensor", false)
    map:block_set_enabled("weak_floor_block", false)
  end

  -- blocks necessary to go back when found the feather
  if map:get_game():get_boolean(517) then
    map:block_set_position("s_block_1", 464, 773)
    map:block_set_position("s_block_2", 496, 773)
  end
end

function map:on_map_opening_transition_finished(destination_point_name)

  -- show the welcome message
  if destination_point_name == "from_roof"
      or destination_point_name == "from_outside_w"
      or destination_point_name == "from_outside_e" then
    map:dialog_start("dungeon_5.welcome")
  end
end

function init_guard(guard_name, path)

  local sprite = map:npc_get_sprite(guard_name)
  local m = sol.movement.create("path")
  m:set_path(path)
  m:set_speed(48)
  m:set_loop(true)
  m:set_ignore_obstacles(true)
  map:npc_start_movement(guard_name, m)
  sprite:set_animation("walking")
end

function map:on_hero_on_sensor(sensor_name)

  sensor_check_guard(sensor_name)

  if sensor_name == "puzzle_wrong_sensor" then
    puzzle_next_sensor = 1
  elseif sensor_name == "save_s_blocks_sensor" then
    if map:get_game():has_item("feather")
        and not map:get_game():get_boolean(517) then
      -- solved the blocks puzzle necessary to exit this floor
      map:get_game():set_boolean(517, true)
    end
  else
    local i = string.match(sensor_name, "puzzle_sensor_([1-4])")
    if i ~= nil
        and not map:get_game():get_boolean(519) then
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

function map:on_hero_still_on_sensor(sensor_name)

  sensor_check_guard(sensor_name)
end

function map:on_dialog_finished(dialog_id, answer)

  dialog_check_guard(dialog_id)

  if dialog_id == "dungeon_5.puzzle_solved" then
    map:hero_unfreeze()
  end
end

function map:on_switch_activated(switch_name)

  if switch_name == "statue_switch"
      and not map:door_is_open("statue_door") then
    map:camera_move(432, 536, 250, open_statue_door)
  end
end

function open_statue_door()

  sol.audio.play_sound("secret")
  sol.audio.play_sound("door_open")
  map:door_open("statue_door")
end

-- Returns whether all torches are on
function are_all_torches_on()

  return map:npc_exists("torch_1")
      and map:npc_get_sprite("torch_1"):get_animation() == "lit"
      and map:npc_get_sprite("torch_2"):get_animation() == "lit"
      and map:npc_get_sprite("torch_3"):get_animation() == "lit"
end

-- Makes all torches on forever
function lock_torches()
  map:npc_remove("torch_1")
  map:npc_remove("torch_2")
  map:npc_remove("torch_3")
end

function map:on_update()

  if not map:door_is_open("torches_door")
      and are_all_torches_on() then

    sol.audio.play_sound("secret")
    map:door_open("torches_door")
    lock_torches()
  end
end

function map:on_sensor_collision_explosion(sensor_name)

  if sensor_name == "weak_floor_sensor"
      and map:tile_is_enabled("weak_floor") then

    map:tile_set_enabled("weak_floor", false)
    map:sensor_set_enabled("weak_floor_sensor", false)
    sol.audio.play_sound("secret")
    map:get_game():set_boolean(515, true)
    sol.timer.start(1500, weak_floor_block_fall)
  end
end

function weak_floor_block_fall()

  map:block_set_enabled("weak_floor_block", false)
  sol.audio.play_sound("jump")
  sol.timer.start(200, weak_floor_block_fall_end)
end

function weak_floor_block_fall_end()

  sol.audio.play_sound("bomb")
end

function puzzle_solved()

  map:hero_freeze()
  sol.audio.play_sound("enemy_awake")
  sol.timer.start(1000, puzzle_solved_2)
end

function puzzle_solved_2()

  sol.timer.start(1000, puzzle_solved_3)
end

function puzzle_solved_3()

  sol.audio.play_sound("secret")
  map:get_game():set_boolean(519, true)
  map:dialog_start("dungeon_5.puzzle_solved")
end

