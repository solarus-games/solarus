local map = ...
-- Bomb cave 2F

function map:on_started(destination_point_name)

  map:door_set_open("door", true)
  if map:get_game():get_boolean(130) then
    map:tile_set_enabled("weak_floor", false)
    map:sensor_set_enabled("weak_floor_sensor", false)
  else
    map:teletransporter_set_enabled("weak_floor_teletransporter", false)
  end
end

function map:on_map_opening_transition_finished(destination_point_name)

  if destination_point_name == "from_3F" then
    map:door_close("door")
  end
end

function map:on_hero_on_sensor(sensor_name)

  if map:door_is_open("door") and sensor_name == "close_door_sensor" then
    map:door_close("door")
  end
end

function map:on_sensor_collision_explosion(sensor_name)

  if sensor_name == "weak_floor_sensor"
      and map:tile_is_enabled("weak_floor") then

    map:tile_set_enabled("weak_floor", false)
    map:sensor_set_enabled("weak_floor_sensor", false)
    map:teletransporter_set_enabled("weak_floor_teletransporter", true)
    sol.audio.play_sound("secret")
    map:get_game():set_boolean(130, true)
  end
end

