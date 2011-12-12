-- Dungeon 3 2F

remove_water_delay = 500 -- delay between each step when some water is disappearing

function event_map_started(destination_point_name)

  if sol.game.savegame_get_boolean(127) then
    -- the barrier of the compass chest is removed
    sol.map.tile_set_enabled("barrier", false)
    sol.map.switch_set_activated("barrier_switch", true)
  end

  if sol.game.savegame_get_boolean(128) then
    -- the south-east water is drained
    sol.map.tile_set_group_enabled("se_water", false)
    sol.map.tile_set_group_enabled("se_water_out", true)
    sol.map.jumper_set_group_enabled("se_water_on_jumper", false)
  else
    sol.map.obstacle_set_group_enabled("se_water_off_obstacle", false)
  end

  if sol.game.savegame_get_boolean(908) then
    -- shortcut A
    sol.map.tile_set_group_enabled("shortcut_a", false)
    sol.map.switch_set_activated("shortcut_a_switch", true)
  end

  if sol.game.savegame_get_boolean(909) then
    -- shortcut B
    sol.map.tile_set_group_enabled("shortcut_b", false)
    sol.map.switch_set_activated("shortcut_b_switch", true)
  end

  -- north chest
  if sol.game.savegame_get_boolean(950) then
    sol.map.switch_set_activated("n_switch", true)
  else
    sol.map.chest_set_enabled("n_chest", false)
  end
end

-- Called when the opening transition of the map finished
function event_map_opening_transition_finished(destination_point_name)

  -- show the welcome message
  if destination_point_name == "from_outside" then
    sol.map.dialog_start("dungeon_3")
  end
end

function event_enemy_dead(enemy_name)

  if string.find(enemy_name, "^e_room_enemy_")
    and sol.map.enemy_is_group_dead("e_room_enemy")
    and not sol.map.door_is_open("e_door") then

    sol.map.camera_move(856, 472, 250, open_e_door)
  end
end

function event_switch_activated(switch_name)

  if switch_name == "barrier_switch" and sol.map.tile_is_enabled("barrier") then
    sol.map.camera_move(120, 240, 250, open_barrier)
  elseif switch_name == "se_water_switch" and not sol.game.savegame_get_boolean(128) then
    sol.map.camera_move(912, 896, 250, remove_se_water, 1000, 3500)
  elseif switch_name == "1f_n_water_switch" and not sol.game.savegame_get_boolean(131) then
    remove_1f_n_water()
  elseif switch_name == "1f_e_water_switch_1"
      and sol.map.switch_is_activated("1f_e_water_switch_2")
      and not sol.game.savegame_get_boolean(122) then
    remove_1f_e_water()
  elseif switch_name == "1f_e_water_switch_2"
      and sol.map.switch_is_activated("1f_e_water_switch_1")
      and not sol.game.savegame_get_boolean(122) then
    remove_1f_e_water()
  elseif switch_name == "shortcut_a_switch" then
    sol.map.tile_set_group_enabled("shortcut_a", false)
    sol.game.savegame_set_boolean(908, true)
    sol.main.play_sound("secret")
  elseif switch_name == "shortcut_b_switch" then
    sol.map.tile_set_group_enabled("shortcut_b", false)
    sol.game.savegame_set_boolean(909, true)
    sol.main.play_sound("secret")
  elseif switch_name == "n_switch" then
    sol.map.camera_move(280, 56, 250, function()
      sol.main.play_sound("chest_appears")
      sol.map.chest_set_enabled("n_chest", true)
      sol.game.savegame_set_boolean(950, true)
    end)
  end
end

function open_e_door()
  sol.main.play_sound("secret")
  sol.map.door_open("e_door")
end

function open_barrier()
  sol.main.play_sound("secret")
  sol.map.tile_set_enabled("barrier", false)
  sol.game.savegame_set_boolean(127, true)
end

function remove_se_water()
  sol.main.play_sound("water_drain_begin")
  sol.main.play_sound("water_drain")
  sol.map.tile_set_enabled("se_water_out", true)
  sol.map.tile_set_enabled("se_water_source", false)
  sol.main.timer_start(remove_se_water_2, remove_water_delay)
end

function remove_se_water_2()
  sol.map.tile_set_enabled("se_water_middle", false)
  sol.main.timer_start(remove_se_water_3, remove_water_delay)
end

function remove_se_water_3()
  sol.map.tile_set_enabled("se_water_initial", false)
  sol.map.tile_set_enabled("se_water_less_a", true)
  sol.main.timer_start(remove_se_water_4, remove_water_delay)
end

function remove_se_water_4()
  sol.map.tile_set_enabled("se_water_less_a", false)
  sol.map.tile_set_enabled("se_water_less_b", true)
  sol.main.timer_start(remove_se_water_5, remove_water_delay)
end

function remove_se_water_5()
  sol.map.tile_set_enabled("se_water_less_b", false)
  sol.map.tile_set_enabled("se_water_less_c", true)
  sol.main.timer_start(remove_se_water_6, remove_water_delay)
end

function remove_se_water_6()
  sol.map.tile_set_enabled("se_water_less_c", false)
  sol.map.jumper_set_group_enabled("se_water_on_jumper", false)
  sol.map.obstacle_set_group_enabled("se_water_off_obstacle", true)
  sol.game.savegame_set_boolean(128, true)
  sol.main.play_sound("secret")
end

function remove_1f_n_water()

  sol.main.play_sound("water_drain_begin")
  sol.main.play_sound("water_drain")
  sol.map.dialog_start("dungeon_3.water_drained_somewhere")
  sol.game.savegame_set_boolean(131, true)
end

function remove_1f_e_water()

  sol.main.play_sound("water_drain_begin")
  sol.main.play_sound("water_drain")
  sol.map.dialog_start("dungeon_3.water_drained_somewhere")
  sol.game.savegame_set_boolean(122, true)
end

