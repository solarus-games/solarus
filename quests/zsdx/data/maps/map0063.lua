-- Dungeon 5 2F

sol.main.include("maps/prison_guard")

fighting_miniboss = false
puzzle_next_sensor = 1

function event_map_started(destination_point_name)

  init_guard("guard_3", "666666666666666666644444222222222222222222200000")

  if sol.game.savegame_get_boolean(515) then
    sol.map.tile_set_enabled("weak_floor", false)
    sol.map.sensor_set_enabled("weak_floor_sensor", false)
  end

  sol.map.door_set_open("miniboss_door", true)
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

  if sensor_name == "start_miniboss_sensor"
      and not sol.game.savegame_get_boolean(517)
      and not fighting_miniboss then
    sol.map.door_close("miniboss_door")
    sol.map.hero_freeze()
    sol.main.timer_start(miniboss_timer, 1000)
    fighting_miniboss = true
  elseif sensor_name == "puzzle_wrong_sensor" then
    puzzle_next_sensor = 1
  else
    local i = string.match(sensor_name, "puzzle_sensor_([1-4])")
    if i ~= nil
        and not sol.game.savegame_get_boolean(519) then
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

function event_dialog_finished(first_message_id, answer)

  dialog_check_guard(first_message_id)

  if first_message_id == "dungeon_5.puzzle_solved" then
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

  sol.main.play_sound("secret")
  sol.main.play_sound("door_open")
  sol.map.door_open("statue_door")
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
    sol.map.sensor_set_enabled("weak_floor_sensor", false)
    sol.main.play_sound("secret")
    sol.game.savegame_set_boolean(515, true)
    sol.main.timer_start(weak_floor_block_fall, 1500)
  end
end

function weak_floor_block_fall()

  sol.map.block_set_enabled("weak_floor_block", false)
  sol.main.play_sound("jump")
  sol.main.timer_start(weak_floor_block_fall_end, 200)
end

function weak_floor_block_fall_end()

  sol.main.play_sound("bomb")
end

function miniboss_timer()
  sol.main.play_music("boss.spc")
  sol.map.enemy_set_enabled("miniboss", true)
  sol.map.hero_unfreeze()
end

function event_enemy_dead(enemy_name)

  if enemy_name == "miniboss" then
    sol.main.play_music("castle.spc")
    sol.map.door_open("miniboss_door")
  end
end

function puzzle_solved()

  sol.map.hero_freeze()
  sol.main.play_sound("enemy_awake")
  sol.main.timer_start(puzzle_solved_2, 1000)
end

function puzzle_solved_2()

  sol.main.timer_start(puzzle_solved_3, 1000)
end

function puzzle_solved_3()

  sol.main.play_sound("secret")
  sol.game.savegame_set_boolean(519, true)
  sol.map.dialog_start("dungeon_5.puzzle_solved")
end

