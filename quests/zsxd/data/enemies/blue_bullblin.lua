-- Bee Guard

going_hero = false
being_pushed = false
movement = nil

function get_main_sprite()
  return sol.enemy.get_sprite("enemies/blue_bullblin")
end

function get_sword_sprite()
  return sol.enemy.get_sprite("enemies/blue_bullblin_sword")
end

function event_appear()

  sol.enemy.set_life(4)
  sol.enemy.set_damage(2)
  sol.enemy.create_sprite("enemies/blue_bullblin_sword")
  sol.enemy.create_sprite("enemies/blue_bullblin")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)

  sword_sprite = get_sword_sprite()
  sol.enemy.set_invincible_sprite(sword_sprite)
  sol.enemy.set_attack_consequence_sprite(sword_sprite, "sword", "custom")
end

function event_restart()

  if not being_pushed then
    if going_hero then
      go_hero()
    else
      go_random()
      check_hero()
    end
  end
end

function event_hurt()
  sol.main.timer_stop("check_hero")
end

function check_hero()

  near_hero = sol.enemy.get_distance_to_hero() < 100
  if near_hero and not going_hero then
    sol.main.play_sound("hero_seen")
    go_hero()
  elseif not near_hero and going_hero then
    go_random()
  end
  sol.main.timer_start(1000, "check_hero", false)
end

function event_movement_changed()

  if not being_pushed then
    movement = sol.enemy.get_movement()
    direction4 = sol.main.movement_get_property(movement, "displayed_direction")
    sol.main.sprite_set_direction(get_main_sprite(), direction4)
    sol.main.sprite_set_direction(get_sword_sprite(), direction4)
  end
end

function event_movement_finished(movement)

  if being_pushed then
    go_hero()
  end
end

function event_custom_attack_received(attack, sprite)

  if attack == "sword" and sprite == get_sword_sprite() then
    sol.main.play_sound("sword_tapping")
    being_pushed = true
    x, y = sol.enemy.get_position()
    hero_x, hero_y = sol.map.hero_get_position()
    angle = sol.main.get_angle(hero_x, hero_y, x, y)
    movement = sol.main.temporal_movement_create(128, angle, 200)
    sol.enemy.start_movement(movement)
  end
end

function go_random()
  movement = sol.main.random_path_movement_create(32)
  sol.enemy.start_movement(movement)
  being_pushed = false
  going_hero = false
end

function go_hero()
  movement = sol.main.target_movement_create(48)
  sol.enemy.start_movement(movement)
  being_pushed = false
  going_hero = true
end

