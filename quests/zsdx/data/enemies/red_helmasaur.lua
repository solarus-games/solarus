function event_appear()

  sol.enemy.set_life(4)
  sol.enemy.set_damage(12)
  sol.enemy.create_sprite("enemies/red_helmasaur")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
end

function event_restart()

  local m = sol.movement.create("path_finding")
  m:set_speed(32)
  sol.enemy.start_movement(m)
end

function event_movement_changed()

  local m = sol.enemy.get_movement()
  local direction4 = m:get_direction4()
  local sprite = sol.enemy.get_sprite()
  sprite:set_direction(direction4)
end

