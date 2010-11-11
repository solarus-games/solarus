function event_use()
  direction = sol.map.hero_get_direction()
  sol.map.hero_start_jumping(direction, 32, false)
  sol.item.set_finished()
end

