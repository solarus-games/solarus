function event_appear()

  sol.enemy.set_life(4)
  sol.enemy.set_damage(4)
  sol.enemy.create_sprite("enemies/red_helmasaur")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
end

function event_restart()

  local m = sol.main.path_finding_movement_create(32)
  sol.enemy.start_movement(m)
end

function event_movement_changed()

  local m = sol.enemy.get_movement()
  local direction4 = sol.main.movement_get_property(m, "displayed_direction")
  local sprite = sol.enemy.get_sprite()
  sol.main.sprite_set_direction(sprite, direction4)
end

