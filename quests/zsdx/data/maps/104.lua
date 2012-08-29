local map = ...
-- Dungeon 9 B1

local door_timer

function map:on_started(destination_point)

  if destination_point:get_name() == "from_1f_w" then
    map:set_doors_open("door", true)
    door_switch:set_activated(true)
  end
end

function door_switch:on_activated()

  if not door:is_open() then
    sol.audio.play_sound("secret")
    map:open_doors("door")
    door_timer = sol.timer.start(12000, function()
      map:close_doors("door")
      map:get_entity(switch_name):set_activated(false)
    end)
    door_timer:set_with_sound(true)
  end
end

function door_dont_close_sensor:on_activated()
  -- disable the timer that would close the door
  if door_timer ~= nil then
    door_timer:stop()
    door_timer = nil
  end
  self:set_enabled(false)
end

