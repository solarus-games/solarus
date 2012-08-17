local map = ...
-- The end

function map:on_started(destination_point_name)

  if destination_point_name == "from_ending" then
    map:hero_freeze()
    map:hero_set_visible(false)
    map:get_game():set_hud_enabled(false)
    map:set_pause_enabled(false)
    sol.timer.start(25000, function()
      map:get_game().reset()
    end)
  end
end

