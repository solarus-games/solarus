-- The end

function event_map_started(destination_point_name)

  if destination_point_name == "from_ending" then
    sol.map.hero_freeze()
    sol.map.hero_set_visible(false)
    sol.map.hud_set_enabled(false)
    sol.map.hud_set_pause_enabled(false)
    sol.main:start_timer(25000, function()
      sol.map.get_game().reset()
    end)
  end
end

