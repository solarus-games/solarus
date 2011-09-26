-- Cave of the hole near the flower

function event_map_started(destination_point_name)

  if destination_point_name ~= "main_entrance" then
    sol.map.door_set_open("door", true)
  end
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "close_door_sensor"
      and sol.map.door_is_open("door") then
    sol.map.door_close("door")
  end
end

