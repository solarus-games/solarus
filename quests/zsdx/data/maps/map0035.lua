local map = ...
-- Bomb cave 2F

function map:on_started(destination_point)

  map:set_doors_open("door", true)
  if map:get_game():get_boolean(130) then
    weak_floor:set_enabled(false)
    weak_floor_sensor:set_enabled(false)
  else
    weak_floor_teletransporter:set_enabled(false)
  end
end

function map:on_opening_transition_finished(destination_point)

  if destination_point:get_name() == "from_3F" then
    map:close_doors("door")
  end
end

function map:on_hero_on_sensor(sensor_name)

  if door:is_open() and sensor_name == "close_door_sensor" then
    map:close_doors("door")
  end
end

function map:on_sensor_collision_explosion(sensor_name)

  if sensor_name == "weak_floor_sensor"
      and weak_floor:is_enabled() then

    weak_floor:set_enabled(false)
    weak_floor_sensor:set_enabled(false)
    weak_floor_teletransporter:set_enabled(true)
    sol.audio.play_sound("secret")
    map:get_game():set_boolean(130, true)
  end
end

