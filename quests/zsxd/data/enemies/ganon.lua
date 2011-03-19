-- Ganon in the temple of stupidities (2F NE)

function event_appear()

  sol.enemy.set_life(100000)
  sol.enemy.set_damage(2)
  sol.enemy.create_sprite("enemies/ganon")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
end

function event_restart()
  m = sol.main.path_finding_movement_create(32)
  sol.enemy.start_movement(m)
end

function event_movement_changed()

  m = sol.enemy.get_movement()
  direction4 = sol.main.movement_get_property(m, "displayed_direction")
  sprite = sol.enemy.get_sprite()
  if direction4 == 1 then
    sol.main.sprite_set_direction(sprite, 1)
  else
    sol.main.sprite_set_direction(sprite, 0)
  end
end

function event_update()

  x, y = sol.enemy.get_position()
  if x > 1216 and sol.enemy.get_life() > 0 then
    sol.enemy.set_layer(0)
    sol.enemy.set_life(0)
    sprite = sol.enemy.get_sprite()
    sol.main.sprite_set_animation(sprite, "hurt")
    sol.main.play_sound("boss_killed")
  end
end

