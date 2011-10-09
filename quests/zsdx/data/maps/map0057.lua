-- Dungeon 6 3F

function event_map_started(destination_point_name)

  sol.map.door_set_open("ne_door", true)
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "ne_door_sensor" then

    if sol.map.door_is_open("ne_door") then
      sol.main.play_sound("door_closed")
      sol.map.door_close("ne_door")
    else
      sol.main.play_sound("door_open")
      sol.map.door_open("ne_door")
    end
  end
end

