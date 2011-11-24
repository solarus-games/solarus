-- Generic script for an enemy that is in a sleep state,
-- and goes towards the the hero when he sees him,
-- and then goes randomly if it loses sight.
-- The enemy has only two new sprites animation: an asleep one,
-- and an awaking transition.
-- a different walking one can be set in the properties, though.

-- Example of use from an enemy script:

-- sol.main.include("enemies/generic_waiting_for_hero")
-- set_properties({
--   sprite = "enemies/globul",
--   life = 4,
--   damage = 2,
--   normal_speed = 32,
--   faster_speed = 48,
--   hurt_style = "normal",
--   push_hero_on_sword = false,
--   pushed_when_hurt = true,
--   asleep_animation = "asleep",
--   awaking_animation = "awaking",
--   normal_animation = "walking",
--   obstacle_behavior = "flying",
--   awakening_sound  = "stone"
-- })

-- The parameter of set_properties() is a table.
-- Its values are all optional except the sprites.

properties = {}
going_hero = false
awaken = false

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
  if properties.hurt_style == nil then
    properties.hurt_style = "normal"
  end
  if properties.pushed_when_hurt == nil then
    properties.pushed_when_hurt = true
  end
  if properties.push_hero_on_sword == nil then
    properties.push_hero_on_sword = false
  end
  if properties.asleep_animation == nil then
    properties.asleep_animation = "asleep"
  end
  if properties.awaking_animation == nil then
    properties.awaking_animation = "awaking"
  end
  if properties.normal_animation == nil then
    properties.normal_animation = "walking"
  end
end

function event_appear()

  sol.enemy.set_life(properties.life)
  sol.enemy.set_damage(properties.damage)
  sol.enemy.create_sprite(properties.sprite)
  sol.enemy.set_hurt_style(properties.hurt_style)
  sol.enemy.set_pushed_back_when_hurt(properties.pushed_when_hurt)
  sol.enemy.set_push_hero_on_sword(properties.push_hero_on_sword)
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 12)
  if not properties.obstacle_behavior == nil then
    sol.enemy.set_obstacle_behavior(properties.obstacle_behavior)
  end
  local sprite = sol.enemy.get_sprite()
  sol.main.sprite_set_animation(sprite, properties.asleep_animation)
end

function event_movement_changed()

  local m = sol.enemy.get_movement()
  local direction4 = sol.main.movement_get_property(m, "displayed_direction")
  local sprite = sol.enemy.get_sprite()
  sol.main.sprite_set_direction(sprite, direction4)
end

function event_obstacle_reached(movement)

  if awaken and not going_hero then
    check_hero()
  end
end


function event_restart()

  if not awaken then
    local sprite = sol.enemy.get_sprite()
    sol.main.sprite_set_animation(sprite, properties.asleep_animation)
  end
  check_hero()
end

function event_hurt()

  sol.main.timer_stop_all()
end

function check_hero()

  local near_hero = sol.enemy.get_distance_to_hero() < 100
  if awaken then
    if near_hero and not going_hero then
        go_hero()
    elseif not near_hero and going_hero then
        go_random()
    end
  elseif not awaken and near_hero then
    wake_up()
  end
  sol.main.timer_start(check_hero, 1000)
end

--  The animation of a sprite is finished
function event_sprite_animation_finished(sprite, animation)

  -- if the awakening transition is finished, make the enemy go toward the hero.
  if animation == properties.awaking_animation then
    sol.main.sprite_set_animation(sprite, properties.normal_animation)
    sol.enemy.set_size(16, 16)
    sol.enemy.set_origin(8, 13)
    sol.enemy.snap_to_grid()
    awaken = true
    go_hero()
  end
end

function wake_up()

  sol.enemy.stop_movement()
  local sprite = sol.enemy.get_sprite()
  sol.main.sprite_set_animation(sprite, properties.awaking_animation)
  if properties.awakening_sound ~= nil then
      sol.main.play_sound(properties.awakening_sound)
  end
end

function go_random()

  local m = properties.random_path_movement_create(properties.normal_speed)
  sol.enemy.start_movement(m)
  going_hero = false
end

function go_hero()

  local m = sol.main.target_movement_create(properties.faster_speed)
  sol.enemy.start_movement(m)
  going_hero = true
end
