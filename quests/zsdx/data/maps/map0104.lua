local map = ...
-- Dungeon 9 B1

local door_timer

function map:on_started(destination_point_name)

  if destination_point_name == "from_1f_w" then
    map:set_doors_open("door", true)
    map:switch_set_activated("door_switch", true)
  end
end

function map:on_switch_activated(switch_name)

  if switch_name == "door_switch"
      and not map:door_is_open("door") then
    sol.audio.play_sound("secret")
    map:open_doors("door")
    door_timer = sol.timer.start(12000, true, function()
      map:close_doors("door")
      map:switch_set_activated(switch_name, false)
    end)
  end
end

function map:on_hero_on_sensor(sensor_name)

  if sensor_name == "door_dont_close_sensor" then
    -- disable the timer that would close the door
    if door_timer ~= nil then
      door_timer:stop()
      door_timer = nil
    end
    map:sensor_set_enabled(sensor_name, false)
  end
end

