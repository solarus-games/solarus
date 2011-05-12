------------------
-- Bomb cave 2F --
------------------

function event_map_started(destination_point_name)

  sol.map.door_set_open("door", true)
  if sol.game.savegame_get_boolean(130) then
    sol.map.tile_set_enabled("weak_floor", false)
  else
    sol.map.teletransporter_set_enabled("weak_floor_teletransporter", false)
  end
end

function event_map_opening_transition_finished(destination_point_name)

  if destination_point_name == "from_3F" then
    sol.map.door_close("door")
  end
end

function event_hero_on_sensor(sensor_name)

  if sol.map.door_is_open("door") and sensor_name == "close_door_sensor" then
    sol.map.door_close("door")
  end
end

function event_sensor_collision_explosion(sensor_name)

  if sensor_name == "weak_floor_sensor"
      and sol.map.tile_is_enabled("weak_floor") then

    sol.map.tile_set_enabled("weak_floor", false)
    sol.map.teletransporter_set_enabled("weak_floor_teletransporter", true)
    sol.main.play_sound("secret")
    sol.game.savegame_set_boolean(130, true)
  end
end

