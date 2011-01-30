-- Bee Guard

being_pushed = false

function event_appear()

  sol.enemy.set_life(3)
  sol.enemy.set_damage(2)
  sol.enemy.create_sprite("enemies/bee_guard_sword")
  sol.enemy.create_sprite("enemies/bee_guard")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)

  sword_sprite = get_sword_sprite()
  sol.enemy.set_invincible_sprite(sword_sprite)
  sol.enemy.set_attack_consequence_sprite(sword_sprite, "sword", "custom")

  restart()
end

function event_movement_changed()

  if not being_pushed then
    m = sol.enemy.get_movement()
    direction4 = sol.main.movement_get_property(m, "displayed_direction")
    sol.main.sprite_set_direction(get_main_sprite(), direction4)
    sol.main.sprite_set_direction(get_sword_sprite(), direction4)
  end
end

function get_main_sprite()
  return sol.enemy.get_sprite("enemies/bee_guard")
end

function get_sword_sprite()
  return sol.enemy.get_sprite("enemies/bee_guard_sword")
end

-- The enemy receives an attack whose consequence is "custom"
function event_custom_attack_received(attack, sprite)

  if attack == "sword" and sprite == get_sword_sprite() then
    sol.main.play_sound("sword_tapping")
    being_pushed = true
    x, y = sol.enemy.get_position()
    hero_x, hero_y = sol.map.hero_get_position()
    angle = sol.main.get_angle(hero_x, hero_y, x, y)
    m = sol.main.temporal_movement_create(128, angle, 200)
    sol.enemy.start_movement(m)
  end
end

function event_movement_finished(movement)
  restart()
end

function restart()
  being_pushed = false
  m = sol.main.target_movement_create(48)
  sol.enemy.start_movement(m)
end


