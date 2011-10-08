-- Dungeon 6 2F

camera_timer = nil
remove_water_delay = 500

function event_map_started(destination_point_name)

  -- weak floor
  if sol.game.savegame_get_boolean(314) then
    sol.map.tile_set_enabled("weak_floor", false)
    sol.map.sensor_set_enabled("weak_floor_sensor", false)
  else
    sol.map.teletransporter_set_enabled("weak_floor_teletransporter", false)
  end

  -- water
  if sol.game.savegame_get_boolean(319) then
    sol.map.tile_set_group_enabled("water", false)
    sol.map.jump_sensor_set_group_enabled("water_on_jump_sensor", false)
  else
    sol.map.obstacle_set_group_enabled("water_off_obstacle", false)
  end
end

function event_hero_on_sensor(sensor_name)

  -- fix the position of the hero when he lands on barriers after the feather jump
  if sensor_name == "above_barrier_sensor" then
    local x, y = sol.map.hero_get_position()
    if y > 629 and y < 641 then
      sol.map.hero_set_position(x, 629, 0)
    elseif y >= 641 and y < 653 then
      sol.map.hero_set_position(x, 653, 0)
    end
  end
end

function event_sensor_collision_explosion(sensor_name)

  if sensor_name == "weak_floor_sensor"
      and sol.map.tile_is_enabled("weak_floor") then

    sol.map.tile_set_enabled("weak_floor", false)
    sol.map.sensor_set_enabled("weak_floor_sensor", false)
    sol.map.teletransporter_set_enabled("weak_floor_teletransporter", true)
    sol.main.play_sound("secret")
    sol.game.savegame_set_boolean(314, true)
  end
end

function event_switch_activated(switch_name)

  if switch_name == "water_switch"
      and not sol.game.savegame_get_boolean(319) then
    sol.map.camera_move(616, 192, 250)
    camera_timer = "remove_water"
  end
end

function event_camera_reached_target()
  sol.main.timer_start(1000, camera_timer, false)
end

function remove_water()
  sol.main.play_sound("water_drain_begin")
  sol.main.play_sound("water_drain")
  sol.map.tile_set_enabled("water_less_1", true)
  sol.map.tile_set_enabled("water_full", false)
  sol.main.timer_start(remove_water_delay, "remove_water_2")
end

function remove_water_2()
  sol.map.tile_set_enabled("water_less_2", true)
  sol.map.tile_set_enabled("water_less_1", false)
  sol.main.timer_start(remove_water_delay, "remove_water_3")
end

function remove_water_3()
  sol.map.tile_set_enabled("water_less_3", true)
  sol.map.tile_set_enabled("water_less_2", false)
  sol.main.timer_start(remove_water_delay, "remove_water_4")
end

function remove_water_4()
  sol.map.tile_set_enabled("water_less_3", false)
  sol.map.jump_sensor_set_group_enabled("water_on_jump_sensor", false)
  sol.map.obstacle_set_group_enabled("water_off_obstacle", true)
  sol.game.savegame_set_boolean(319, true)
  sol.main.play_sound("secret")
  sol.main.timer_start(1000, "sol.map.camera_restore", false)
end

