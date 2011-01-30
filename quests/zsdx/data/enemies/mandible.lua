-- Globul

function event_appear()

  sol.enemy.set_life(5)
  sol.enemy.set_damage(4)
  sol.enemy.create_sprite("enemies/mandible")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
  sol.enemy.set_hurt_sound_style("monster")

  m = sol.main.target_movement_create(48)
  sol.enemy.start_movement(m)
end

function event_movement_changed()

  m = sol.enemy.get_movement()
  direction4 = sol.main.movement_get_property(m, "displayed_direction")
  sprite = sol.enemy.get_sprite()
  sol.main.sprite_set_direction(sprite, direction4)
end


