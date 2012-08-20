local map = ...
-- Dungeon 3 2F

remove_water_delay = 500 -- delay between each step when some water is disappearing

function map:on_started(destination_point_name)

  if map:get_game():get_boolean(127) then
    -- the barrier of the compass chest is removed
    map:tile_set_enabled("barrier", false)
    map:switch_set_activated("barrier_switch", true)
  end

  if map:get_game():get_boolean(128) then
    -- the south-east water is drained
    map:tile_set_group_enabled("se_water", false)
    map:tile_set_group_enabled("se_water_out", true)
    map:jumper_set_group_enabled("se_water_on_jumper", false)
  else
    map:wall_set_group_enabled("se_water_off_obstacle", false)
  end

  if map:get_game():get_boolean(908) then
    -- shortcut A
    map:tile_set_group_enabled("shortcut_a", false)
    map:switch_set_activated("shortcut_a_switch", true)
  end

  if map:get_game():get_boolean(909) then
    -- shortcut B
    map:tile_set_group_enabled("shortcut_b", false)
    map:switch_set_activated("shortcut_b_switch", true)
  end

  -- north chest
  if map:get_game():get_boolean(950) then
    map:switch_set_activated("n_switch", true)
  else
    map:chest_set_enabled("n_chest", false)
  end
end

-- Called when the opening transition of the map finished
function map:on_map_opening_transition_finished(destination_point_name)

  -- show the welcome message
  if destination_point_name == "from_outside" then
    map:dialog_start("dungeon_3")
  end
end

function map:on_enemy_dead(enemy_name)

  if string.find(enemy_name, "^e_room_enemy_")
    and map:enemy_is_group_dead("e_room_enemy")
    and not map:door_is_open("e_door") then

    map:camera_move(856, 472, 250, open_e_door)
  end
end

function map:on_switch_activated(switch_name)

  if switch_name == "barrier_switch" and map:tile_is_enabled("barrier") then
    map:camera_move(120, 240, 250, open_barrier)
  elseif switch_name == "se_water_switch" and not map:get_game():get_boolean(128) then
    map:camera_move(912, 896, 250, remove_se_water, 1000, 3500)
  elseif switch_name == "1f_n_water_switch" and not map:get_game():get_boolean(131) then
    remove_1f_n_water()
  elseif switch_name == "1f_e_water_switch_1"
      and map:switch_is_activated("1f_e_water_switch_2")
      and not map:get_game():get_boolean(122) then
    remove_1f_e_water()
  elseif switch_name == "1f_e_water_switch_2"
      and map:switch_is_activated("1f_e_water_switch_1")
      and not map:get_game():get_boolean(122) then
    remove_1f_e_water()
  elseif switch_name == "shortcut_a_switch" then
    map:tile_set_group_enabled("shortcut_a", false)
    map:get_game():set_boolean(908, true)
    sol.audio.play_sound("secret")
  elseif switch_name == "shortcut_b_switch" then
    map:tile_set_group_enabled("shortcut_b", false)
    map:get_game():set_boolean(909, true)
    sol.audio.play_sound("secret")
  elseif switch_name == "n_switch" then
    map:camera_move(280, 56, 250, function()
      sol.audio.play_sound("chest_appears")
      map:chest_set_enabled("n_chest", true)
      map:get_game():set_boolean(950, true)
    end)
  end
end

function open_e_door()
  sol.audio.play_sound("secret")
  map:door_open("e_door")
end

function open_barrier()
  sol.audio.play_sound("secret")
  map:tile_set_enabled("barrier", false)
  map:get_game():set_boolean(127, true)
end

function remove_se_water()
  sol.audio.play_sound("water_drain_begin")
  sol.audio.play_sound("water_drain")
  map:tile_set_enabled("se_water_out", true)
  map:tile_set_enabled("se_water_source", false)
  sol.timer.start(remove_water_delay, remove_se_water_2)
end

function remove_se_water_2()
  map:tile_set_enabled("se_water_middle", false)
  sol.timer.start(remove_water_delay, remove_se_water_3)
end

function remove_se_water_3()
  map:tile_set_enabled("se_water_initial", false)
  map:tile_set_enabled("se_water_less_a", true)
  sol.timer.start(remove_water_delay, remove_se_water_4)
end

function remove_se_water_4()
  map:tile_set_enabled("se_water_less_a", false)
  map:tile_set_enabled("se_water_less_b", true)
  sol.timer.start(remove_water_delay, remove_se_water_5)
end

function remove_se_water_5()
  map:tile_set_enabled("se_water_less_b", false)
  map:tile_set_enabled("se_water_less_c", true)
  sol.timer.start(remove_water_delay, remove_se_water_6)
end

function remove_se_water_6()
  map:tile_set_enabled("se_water_less_c", false)
  map:jumper_set_group_enabled("se_water_on_jumper", false)
  map:wall_set_group_enabled("se_water_off_obstacle", true)
  map:get_game():set_boolean(128, true)
  sol.audio.play_sound("secret")
end

function remove_1f_n_water()

  sol.audio.play_sound("water_drain_begin")
  sol.audio.play_sound("water_drain")
  map:dialog_start("dungeon_3.water_drained_somewhere")
  map:get_game():set_boolean(131, true)
end

function remove_1f_e_water()

  sol.audio.play_sound("water_drain_begin")
  sol.audio.play_sound("water_drain")
  map:dialog_start("dungeon_3.water_drained_somewhere")
  map:get_game():set_boolean(122, true)
end

