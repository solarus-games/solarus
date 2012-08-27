local map = ...
-- Fairy cave 2F

function map:on_started(destination_point)

  if map:get_game():get_boolean(953) then
    map:set_entities_enabled("barrier", false)
    barrier_switch:set_activated(true)
  end
end

function map:on_switch_activated(switch_name)

  if switch_name == "barrier_switch" then
    map:move_camera(312, 568, 250, function()
      sol.audio.play_sound("secret")
      map:get_game():set_boolean(953, true)
      map:set_entities_enabled("barrier", false)
    end)
  end
end

