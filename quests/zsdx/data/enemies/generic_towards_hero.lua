-- Generic script for an enemy that goes towards the
-- the hero if he sees him, and walks randomly otherwise.
-- The enemy has only one sprite. See generic_soldier.lua
-- for an enemy with a sword.

-- Example of use from an enemy script:

-- sol.main.include("enemies/generic_towards_hero")
-- set_properties({
--   sprite = "enemies/globul",
--   life = 4,
--   damage = 2,
--   normal_speed = 32,
--   faster_speed = 48,
--   hurt_sound_style = "normal"
-- })

-- The parameter of set_properties() is an array.
-- Its values are all optional except main_sprite
-- and sword_sprite.

properties = {}
going_hero = false

function set_properties(prop)

  properties = prop
  -- set default values
  if properties.life == nil then
    properties.life = 2
  end
  if properties.damage == nil then
    properties.damage = 2
  end
  if properties.normal_speed == nil then
    properties.normal_speed = 32
  end
  if properties.faster_speed == nil then
    properties.faster_speed = 48
  end
  if properties.hurt_sound_style == nil then
    properties.hurt_sound_style = "normal"
  end
end

function event_appear()

  sol.enemy.set_life(properties.life)
  sol.enemy.set_damage(properties.damage)
  sol.enemy.create_sprite(properties.sprite)
  sol.enemy.set_hurt_sound_style(properties.hurt_sound_style)
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
end

function event_movement_changed()

  m = sol.enemy.get_movement()
  direction4 = sol.main.movement_get_property(m, "displayed_direction")
  sprite = sol.enemy.get_sprite()
  sol.main.sprite_set_direction(sprite, direction4)
end

function event_restart()
  go_random()
  check_hero()
end

function event_hurt()
  sol.main.timer_stop("check_hero")
end

function check_hero()

  near_hero = sol.enemy.get_distance_to_hero() < 100
  if near_hero and not going_hero then
    go_hero()
  elseif not near_hero and going_hero then
    go_random()
  end
  sol.main.timer_start(1000, "check_hero", false)
end

function go_random()
  m = sol.main.random_path_movement_create(properties.normal_speed)
  sol.enemy.start_movement(m)
  going_hero = false
end

function go_hero()
  m = sol.main.target_movement_create(properties.faster_speed)
  sol.enemy.start_movement(m)
  going_hero = true
end

