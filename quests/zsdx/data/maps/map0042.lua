-- Dungeon 3 3F

function event_map_started(destination_point_name)

  sol.map.light_set(0)
  if sol.game.savegame_get_boolean(133) then
    sol.map.tile_set_enabled("weak_floor", false)
    sol.map.sensor_set_enabled("weak_floor_sensor", false)
  else
    sol.map.teletransporter_set_enabled("weak_floor_teletransporter", false)
  end
end

function event_switch_activated(switch_name)

  if switch_name == "se_door_switch" and not sol.map.door_is_open("se_door") then
    sol.main.play_sound("secret")
    sol.map.door_open("se_door")
  elseif switch_name == "nc_door_switch" and not sol.map.door_is_open("nc_door") then
    sol.main.play_sound("secret")
    sol.map.door_open("nc_door")
  elseif switch_name == "barrier_switch" and sol.map.tile_is_enabled("barrier") then
    sol.map.tile_set_enabled("barrier", false)
    sol.main.play_sound("secret")
  end
end

function event_sensor_collision_explosion(sensor_name)

  if sensor_name == "weak_floor_sensor"
      and sol.map.tile_is_enabled("weak_floor") then

    sol.map.tile_set_enabled("weak_floor", false)
    sol.map.sensor_set_enabled("weak_floor_sensor", false)
    sol.map.teletransporter_set_enabled("weak_floor_teletransporter", true)
    sol.main.play_sound("secret")
    sol.game.savegame_set_boolean(133, true)
  end
end

