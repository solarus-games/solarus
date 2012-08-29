local map = ...
-- The end

function map:on_started(destination)

  if destination:get_name() == "from_ending" then
    hero:freeze()
    hero:set_visible(false)
    map:get_game():set_hud_enabled(false)
    map:set_pause_enabled(false)
    sol.timer.start(25000, function()
      map:get_game().reset()
    end)
  end
end

