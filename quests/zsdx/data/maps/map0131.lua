-- The end

function event_map_started(destination_point_name)

  if destination_point_name == "from_ending" then
    sol.map.hero_freeze()
    sol.map.hero_set_visible(false)
    sol.map.hud_set_enabled(false)
    sol.map.hud_set_pause_enabled(false)
    sol.main.timer_start(sol.game.reset, 25000)
  end
end

