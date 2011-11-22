-- Generic script for an enemy with a sword
-- that goes towards the hero if he sees him
-- and walks randomly otherwise.

-- Example of use from an enemy script:

-- sol.main.include("enemies/generic_soldier")
-- set_properties({
--   main_sprite = "enemies/green_knight_soldier",
--   sword_sprite = "enemies/green_knight_soldier_sword",
--   life = 4,
--   damage = 2,
--   play_hero_seen_sound = false,
--   normal_speed = 32,
--   faster_speed = 64,
--   hurt_style = "normal"
-- })

-- The parameter of set_properties() is a table.
-- Its values are all optional except main_sprite
-- and sword_sprite.

properties = {}
going_hero = false
being_pushed = false
movement = nil
main_sprite = nil
sword_sprite = nil

function set_properties(prop)

  properties = prop
  -- set default values
  if properties.life == nil then
    properties.life = 2
  end
  if properties.damage == nil then
    properties.damage = 2
  end
  if properties.play_hero_seen_sound == nil then
    properties.play_hero_seen_sound = false
  end
  if properties.normal_speed == nil then
    properties.normal_speed = 32
  end
  if properties.faster_speed == nil then
    properties.faster_speed = 64
  end
  if properties.hurt_style == nil then
    properties.hurt_style = "normal"
  end
end

function event_appear()

  sol.enemy.set_life(properties.life)
  sol.enemy.set_damage(properties.damage)
  sol.enemy.set_hurt_style(properties.hurt_style)
  sword_sprite = sol.enemy.create_sprite(properties.sword_sprite)
  main_sprite = sol.enemy.create_sprite(properties.main_sprite)
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)

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
  sol.main.timer_stop_all()
end

function check_hero()

  local near_hero = sol.enemy.get_distance_to_hero() < 100
  if near_hero and not going_hero then
    if properties.play_hero_seen_sound then
      sol.main.play_sound("hero_seen")
    end
    go_hero()
  elseif not near_hero and going_hero then
    go_random()
  end
  sol.main.timer_start(check_hero, 1000)
end

function event_movement_changed()

  if not being_pushed then
    movement = sol.enemy.get_movement()
    local direction4 = sol.main.movement_get_property(movement, "displayed_direction")
    sol.main.sprite_set_direction(main_sprite, direction4)
    sol.main.sprite_set_direction(sword_sprite, direction4)
  end
end

function event_movement_finished(movement)

  if being_pushed then
    go_hero()
  end
end

function event_obstacle_reached(movement)

  if being_pushed then
    go_hero()
  end
end

function event_custom_attack_received(attack, sprite)

  if attack == "sword" and sprite == sword_sprite then
    sol.main.play_sound("sword_tapping")
    being_pushed = true
    local x, y = sol.enemy.get_position()
    local hero_x, hero_y = sol.map.hero_get_position()
    local angle = sol.main.get_angle(hero_x, hero_y, x, y)
    movement = sol.main.straight_movement_create(128, angle)
    sol.main.movement_set_property(movement, "max_distance", 26)
    sol.main.movement_set_property(movement, "smooth", true)
    sol.enemy.start_movement(movement)
    return true
  end

  -- tell the engine that the attack is not handled
  return true
end

function go_random()
  movement = sol.main.random_path_movement_create(properties.normal_speed)
  sol.enemy.start_movement(movement)
  being_pushed = false
  going_hero = false
end

function go_hero()
  movement = sol.main.target_movement_create(properties.faster_speed)
  sol.enemy.start_movement(movement)
  being_pushed = false
  going_hero = true
end

