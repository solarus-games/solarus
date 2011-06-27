-- Dungeon 3 2F

camera_timer = nil
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
  end

end

function event_enemy_dead(enemy_name)

  if string.match(enemy_name, "^e_room_enemy_")
    and sol.map.enemy_is_group_dead("e_room_enemy")
    and not sol.map.door_is_open("e_door") then

    camera_timer = "open_e_door"
    sol.map.camera_move(856, 472, 150)
  end
end

function event_switch_activated(switch_name)

  if switch_name == "barrier_switch" and sol.map.tile_is_enabled("barrier") then
    camera_timer = "open_barrier"
    sol.map.camera_move(120, 240, 150)
  elseif switch_name == "se_water_switch" and not sol.game.savegame_get_boolean(128) then
    sol.map.camera_move(912, 896, 150)
    camera_timer = "remove_se_water"
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
  end
end

function event_camera_reached_target()
  sol.main.timer_start(1000, camera_timer, false)
end

function open_e_door()
  sol.main.play_sound("secret")
  sol.map.door_open("e_door")
  sol.main.timer_start(1000, "sol.map.camera_restore", false)
end

function open_barrier()
  sol.main.play_sound("secret")
  sol.map.tile_set_enabled("barrier", false)
  sol.game.savegame_set_boolean(127, true)
  sol.main.timer_start(1000, "sol.map.camera_restore", false)
end

function remove_se_water()
  sol.main.play_sound("water_drain_begin")
  sol.main.play_sound("water_drain")
  sol.map.tile_set_enabled("se_water_out", true)
  sol.map.tile_set_enabled("se_water_source", false)
  sol.main.timer_start(remove_water_delay, "remove_se_water_2", false)
end

function remove_se_water_2()
  sol.map.tile_set_enabled("se_water_middle", false)
  sol.main.timer_start(remove_water_delay, "remove_se_water_3", false)
end

function remove_se_water_3()
  sol.map.tile_set_enabled("se_water_initial", false)
  sol.map.tile_set_enabled("se_water_less_a", true)
  sol.main.timer_start(remove_water_delay, "remove_se_water_4", false)
end

function remove_se_water_4()
  sol.map.tile_set_enabled("se_water_less_a", false)
  sol.map.tile_set_enabled("se_water_less_b", true)
  sol.main.timer_start(remove_water_delay, "remove_se_water_5", false)
end

function remove_se_water_5()
  sol.map.tile_set_enabled("se_water_less_b", false)
  sol.map.tile_set_enabled("se_water_less_c", true)
  sol.main.timer_start(remove_water_delay, "remove_se_water_6", false)
end

function remove_se_water_6()
  sol.map.tile_set_enabled("se_water_less_c", false)
  sol.game.savegame_set_boolean(128, true)
  sol.main.play_sound("secret")
  sol.main.timer_start(1000, "sol.map.camera_restore", false)
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

