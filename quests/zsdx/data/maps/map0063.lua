local map = ...
-- Dungeon 5 2F

sol.main.do_file("maps/prison_guard")

puzzle_next_sensor = 1

function map:on_started(destination_point)

  init_guard("guard_3", {6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,4,4,4,4,4,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0})

  if map:get_game():get_boolean(515) then
    weak_floor:set_enabled(false)
    weak_floor_sensor:set_enabled(false)
    weak_floor_block:set_enabled(false)
  end

  -- blocks necessary to go back when found the feather
  if map:get_game():get_boolean(517) then
    s_block_1:set_position(464, 773)
    s_block_2:set_position(496, 773)
  end
end

function map:on_opening_transition_finished(destination_point)

  -- show the welcome message
  if destination_point:get_name() == "from_roof"
      or destination_point == "from_outside_w"
      or destination_point == "from_outside_e" then
    map:start_dialog("dungeon_5.welcome")
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
    map:get_hero():unfreeze()
  end
end

function map:on_switch_activated(switch_name)

  if switch_name == "statue_switch"
      and not statue_door:is_open() then
    map:move_camera(432, 536, 250, open_statue_door)
  end
end

function open_statue_door()

  sol.audio.play_sound("secret")
  sol.audio.play_sound("door_open")
  map:open_doors("statue_door")
end

-- Returns whether all torches are on
function are_all_torches_on()

  return torch_1:exists()
      and torch_1:get_sprite():get_animation() == "lit"
      and torch_2:get_sprite():get_animation() == "lit"
      and torch_3:get_sprite():get_animation() == "lit"
end

-- Makes all torches on forever
function lock_torches()
  torch_1:remove()
  torch_2:remove()
  torch_3:remove()
end

function map:on_update()

  if not torches_door:is_open()
      and are_all_torches_on() then

    sol.audio.play_sound("secret")
    map:open_doors("torches_door")
    lock_torches()
  end
end

function map:on_sensor_collision_explosion(sensor_name)

  if sensor_name == "weak_floor_sensor"
      and weak_floor:is_enabled() then

    weak_floor:set_enabled(false)
    weak_floor_sensor:set_enabled(false)
    sol.audio.play_sound("secret")
    map:get_game():set_boolean(515, true)
    sol.timer.start(1500, weak_floor_block_fall)
  end
end

function weak_floor_block_fall()

  weak_floor_block:set_enabled(false)
  sol.audio.play_sound("jump")
  sol.timer.start(200, weak_floor_block_fall_end)
end

function weak_floor_block_fall_end()

  sol.audio.play_sound("bomb")
end

function puzzle_solved()

  map:get_hero():freeze()
  sol.audio.play_sound("enemy_awake")
  sol.timer.start(1000, puzzle_solved_2)
end

function puzzle_solved_2()

  sol.timer.start(1000, puzzle_solved_3)
end

function puzzle_solved_3()

  sol.audio.play_sound("secret")
  map:get_game():set_boolean(519, true)
  map:start_dialog("dungeon_5.puzzle_solved")
end

