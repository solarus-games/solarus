------------------
-- Bomb cave 2F --
------------------

function event_map_started(destination_point_name)
  sol.map.door_set_open("door", true)
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "close_door_sensor" then
    sol.map.door_close("door")
  end
end

function event_map_opening_transition_finished(destination_point_name)

  if destination_point_name == "from_3F" then
    sol.map.door_close("door")
  end
end

