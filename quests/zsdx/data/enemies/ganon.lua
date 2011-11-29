-- Ganon
-- phase 1: periodically throws flames to the hero
-- phase 2: also periodically throws a set of fire bats in circles or randomly
--          or towards the hero

being_pushed = false

function event_appear()

  sol.enemy.set_life(1)
  sol.enemy.set_damage(24)
  sol.enemy.create_sprite("enemies/ganon")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
  --sol.enemy.set_invincible()
  --sol.enemy.set_attack_consequence("sword", "custom")
  sol.enemy.set_push_hero_on_sword(true)
end

function event_restart()
  m = sol.main.path_finding_movement_create(32)
  sol.enemy.start_movement(m)
end

function event_movement_changed()

  if not being_pushed then
    m = sol.enemy.get_movement()
    direction4 = sol.main.movement_get_property(m, "displayed_direction")
    sprite = sol.enemy.get_sprite()
    if direction4 == 1 then
      sol.main.sprite_set_direction(sprite, 1)
    else
      sol.main.sprite_set_direction(sprite, 0)
    end
  end
end

function event_custom_attack_received(attack, sprite)

  if attack == "sword" then
    sol.main.play_sound("sword_tapping")
    being_pushed = true
    x, y = sol.enemy.get_position()
    hero_x, hero_y = sol.map.hero_get_position()
    angle = sol.main.get_angle(hero_x, hero_y, x, y)
    movement = sol.main.straight_movement_create(128, angle)
    sol.main.movement_set_property(movement, "max_distance", 28)
    sol.enemy.start_movement(movement)
  end
end

function event_movement_finished(movement)

  if being_pushed then
    event_restart()
  end
end

function event_obstacle_reached(movement)

  if being_pushed then
    event_restart()
  end
end

