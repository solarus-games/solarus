-- Gelidrak: an ice dragon from Newlink

function event_appear()

  sol.enemy.set_life(8)
  sol.enemy.set_damage(1)
  sol.enemy.create_sprite("enemies/gelidrak")
  sol.enemy.set_size(240, 112)
  sol.enemy.set_origin(120, 88)
  sol.enemy.set_invincible()
end

function event_restart()

  local m = sol.main.random_movement_create(32)
  sol.enemy.start_movement(m)
end

