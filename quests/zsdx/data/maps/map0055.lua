local map = ...
-- Dungeon 6 2F

remove_water_delay = 500

function map:on_started(destination_point_name)

  -- weak floor
  if map:get_game():get_boolean(314) then
    map:tile_set_enabled("weak_floor", false)
    map:sensor_set_enabled("weak_floor_sensor", false)
  else
    map:teletransporter_set_enabled("weak_floor_teletransporter", false)
  end

  -- water
  if map:get_game():get_boolean(319) then
    map:tile_set_group_enabled("water", false)
    map:jumper_set_group_enabled("water_on_jumper", false)
    map:switch_set_activated("water_switch", true)
  else
    map:obstacle_set_group_enabled("water_off_obstacle", false)
  end
end

function map:on_hero_on_sensor(sensor_name)

  -- fix the position of the hero when he lands on barriers after the feather jump
  if sensor_name == "above_barrier_sensor" then
    local x, y = map:hero_get_position()
    if y > 629 and y < 641 then
      map:hero_set_position(x, 629, 0)
    elseif y >= 641 and y < 653 then
      map:hero_set_position(x, 653, 0)
    end
  end
end

function map:on_sensor_collision_explosion(sensor_name)

  if sensor_name == "weak_floor_sensor"
      and map:tile_is_enabled("weak_floor") then

    map:tile_set_enabled("weak_floor", false)
    map:sensor_set_enabled("weak_floor_sensor", false)
    map:teletransporter_set_enabled("weak_floor_teletransporter", true)
    sol.audio.play_sound("secret")
    map:get_game():set_boolean(314, true)
  end
end

function map:on_switch_activated(switch_name)

  if switch_name == "water_switch"
      and not map:get_game():get_boolean(319) then
    map:camera_move(616, 192, 250, remove_water, 1000, 2500)
  end
end

function remove_water()
  sol.audio.play_sound("water_drain_begin")
  sol.audio.play_sound("water_drain")
  map:tile_set_enabled("water_less_1", true)
  map:tile_set_enabled("water_full", false)
  sol.timer.start(remove_water_delay, remove_water_2)
end

function remove_water_2()
  map:tile_set_enabled("water_less_2", true)
  map:tile_set_enabled("water_less_1", false)
  sol.timer.start(remove_water_delay, remove_water_3)
end

function remove_water_3()
  map:tile_set_enabled("water_less_3", true)
  map:tile_set_enabled("water_less_2", false)
  sol.timer.start(remove_water_delay, remove_water_4)
end

function remove_water_4()
  map:tile_set_enabled("water_less_3", false)
  map:jumper_set_group_enabled("water_on_jumper", false)
  map:obstacle_set_group_enabled("water_off_obstacle", true)
  map:get_game():set_boolean(319, true)
  sol.audio.play_sound("secret")
end

