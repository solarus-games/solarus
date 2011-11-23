-- Dungeon 9 B1

function event_map_started(destination_point_name)

  if destination_point_name == "from_1f_w" then
    sol.map.door_set_open("door", true)
    sol.map.switch_set_activated("door_switch", true)
  end
end

function event_switch_activated(switch_name)

  if switch_name == "door_switch"
      and not sol.map.door_is_open("door") then
    sol.main.play_sound("secret")
    sol.map.door_open("door")
    sol.main.timer_start(function()
      if door_allow_close then
        sol.map.door_close("door")
        sol.map.switch_set_activated(switch_name, true)
      end
    end, 12000, true)
  end
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "door_dont_close_sensor" then
    -- disable the timer that would close the door
    sol.main.timer_stop_all()
    sol.map.sensor_set_enabled(sensor_name, false)
  end
end

