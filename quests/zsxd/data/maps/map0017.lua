-- The end

function event_map_started(destination_point_name)

  sol.map.hud_set_enabled(false)
  sol.map.hero_freeze()
  sol.game.save()
  sol.main.timer_start(10000, "sol.game.reset", false)
  sol.main.play_sound("hero_dying")
end

