local map = ...
-- Dungeon 3 3F

function map:on_started(destination_point_name)

  map:set_light(0)

  -- weak floor
  if map:get_game():get_boolean(133) then
    map:tile_set_enabled("weak_floor", false)
    map:sensor_set_enabled("weak_floor_sensor", false)
  else
    map:teletransporter_set_enabled("weak_floor_teletransporter", false)
  end

  -- piece of heart
  if map:get_game():get_boolean(132) then
    map:tile_set_enabled("barrier", false)
    map:switch_set_activated("barrier_switch", true)
  end
end

function map:on_switch_activated(switch_name)

  if switch_name == "se_door_switch" and not map:door_is_open("se_door") then
    sol.audio.play_sound("secret")
    map:door_open("se_door")
  elseif switch_name == "nc_door_switch" and not map:door_is_open("nc_door") then
    sol.audio.play_sound("secret")
    map:door_open("nc_door")
  elseif switch_name == "barrier_switch" and map:tile_is_enabled("barrier") then
    map:tile_set_enabled("barrier", false)
    sol.audio.play_sound("door_open")
  end
end

function map:on_sensor_collision_explosion(sensor_name)

  if sensor_name == "weak_floor_sensor"
      and map:tile_is_enabled("weak_floor") then

    map:tile_set_enabled("weak_floor", false)
    map:sensor_set_enabled("weak_floor_sensor", false)
    map:teletransporter_set_enabled("weak_floor_teletransporter", true)
    sol.audio.play_sound("secret")
    map:get_game():set_boolean(133, true)
  end
end

