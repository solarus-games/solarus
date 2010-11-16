function event_use()
  sol.main.play_sound("jump")
  direction4 = sol.map.hero_get_direction()
  sol.map.hero_start_jumping(direction4 * 2, 32, false)
  sol.item.set_finished()
end

