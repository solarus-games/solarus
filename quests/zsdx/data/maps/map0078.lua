local map = ...
-- Fairy cave 2F

function map:on_started(destination_point_name)

  if map:get_game():get_boolean(953) then
    map:tile_set_group_enabled("barrier", false)
    map:switch_set_activated("barrier_switch", true)
  end
end

function map:on_switch_activated(switch_name)

  if switch_name == "barrier_switch" then
    map:move_camera(312, 568, 250, function()
      sol.audio.play_sound("secret")
      map:get_game():set_boolean(953, true)
      map:tile_set_group_enabled("barrier", false)
    end)
  end
end

