-- Zelda

function event_appear()

  sol.enemy.set_life(100)
  sol.enemy.set_damage(4)
  sol.enemy.create_sprite("enemies/zelda")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
  sol.enemy.set_invincible()
end

function event_restart()
  m = sol.main.path_finding_movement_create(64)
  sol.enemy.start_movement(m)
end

function event_movement_changed()

  m = sol.enemy.get_movement()
  direction4 = sol.main.movement_get_property(m, "displayed_direction")
  sprite = sol.enemy.get_sprite()
  sol.main.sprite_set_direction(sprite, direction4)
end

