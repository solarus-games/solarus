local map = ...
-- Dungeon 6 2F

remove_water_delay = 500

function map:on_started(destination_point)

  -- weak floor
  if map:get_game():get_boolean(314) then
    weak_floor:set_enabled(false)
    weak_floor_sensor:set_enabled(false)
  else
    weak_floor_teletransporter:set_enabled(false)
  end

  -- water
  if map:get_game():get_boolean(319) then
    map:set_entities_enabled("water", false)
    map:set_entities_enabled("water_on_jumper", false)
    water_switch:set_activated(true)
  else
    map:set_entities_enabled("water_off_obstacle", false)
  end
end

function map:on_hero_on_sensor(sensor_name)

  -- fix the position of the hero when he lands on barriers after the feather jump
  if sensor_name == "above_barrier_sensor" then
    local x, y = hero:get_position()
    if y > 629 and y < 641 then
      hero:set_position(x, 629, 0)
    elseif y >= 641 and y < 653 then
      hero:set_position(x, 653, 0)
    end
  end
end

function map:on_sensor_collision_explosion(sensor_name)

  if sensor_name == "weak_floor_sensor"
      and weak_floor:is_enabled() then

    weak_floor:set_enabled(false)
    weak_floor_sensor:set_enabled(false)
    weak_floor_teletransporter:set_enabled(true)
    sol.audio.play_sound("secret")
    map:get_game():set_boolean(314, true)
  end
end

function map:on_switch_activated(switch_name)

  if switch_name == "water_switch"
      and not map:get_game():get_boolean(319) then
    map:move_camera(616, 192, 250, remove_water, 1000, 2500)
  end
end

function remove_water()
  sol.audio.play_sound("water_drain_begin")
  sol.audio.play_sound("water_drain")
  water_less_1:set_enabled(true)
  water_full:set_enabled(false)
  sol.timer.start(remove_water_delay, remove_water_2)
end

function remove_water_2()
  water_less_2:set_enabled(true)
  water_less_1:set_enabled(false)
  sol.timer.start(remove_water_delay, remove_water_3)
end

function remove_water_3()
  water_less_3:set_enabled(true)
  water_less_2:set_enabled(false)
  sol.timer.start(remove_water_delay, remove_water_4)
end

function remove_water_4()
  water_less_3:set_enabled(false)
  map:set_entities_enabled("water_on_jumper", false)
  map:set_entities_enabled("water_off_obstacle", true)
  map:get_game():set_boolean(319, true)
  sol.audio.play_sound("secret")
end

