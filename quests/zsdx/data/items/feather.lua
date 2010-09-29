function event_use()
  direction = sol.map.hero_get_animation_direction()
  sol.map.hero_jump(direction, 32, false)
  sol.item.set_finished()
end

