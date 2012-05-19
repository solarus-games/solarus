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

local properties = {}
local going_hero = false
local being_pushed = false
local movement = nil
local main_sprite = nil
local sword_sprite = nil
local timer

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
  if timer ~= nil then
    timer:stop()
    timer = nil
  end
end

function check_hero()

  local _, _, layer = sol.enemy.get_position()
  local _, _, hero_layer = sol.map.hero_get_position()
  local near_hero = layer == hero_layer
    and sol.enemy.get_distance_to_hero() < 100

  if near_hero and not going_hero then
    if properties.play_hero_seen_sound then
      sol.audio.play_sound("hero_seen")
    end
    go_hero()
  elseif not near_hero and going_hero then
    go_random()
  end
  timer = sol.main:start_timer(1000, check_hero)
end

function event_movement_changed()

  if not being_pushed then
    movement = sol.enemy.get_movement()
    local direction4 = movement:get_direction4()
    main_sprite:set_direction(direction4)
    sword_sprite:set_direction(direction4)
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
    sol.audio.play_sound("sword_tapping")
    being_pushed = true
    local x, y = sol.enemy.get_position()
    local hero_x, hero_y = sol.map.hero_get_position()
    local angle = sol.main.get_angle(hero_x, hero_y, x, y)
    movement = sol.movement.create("straight")
    movement:set_speed(128)
    movement:set_angle(angle)
    movement:set_max_distance(26)
    movement:set_smooth(true)
    sol.enemy.start_movement(movement)
  end
end

function go_random()
  movement = sol.movement.create("random_path")
  movement:set_speed(properties.normal_speed)
  sol.enemy.start_movement(movement)
  being_pushed = false
  going_hero = false
end

function go_hero()
  movement = sol.movement.create("target")
  movement:set_speed(properties.faster_speed)
  sol.enemy.start_movement(movement)
  being_pushed = false
  going_hero = true
end

