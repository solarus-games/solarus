function event_use()
  sol.main.play_sound("jump")
  direction4 = sol.map.hero_get_direction()

  sol.map.hero_start_jumping(direction4 * 2, math.random(16,200), false)
  sol.item.set_finished()
end

