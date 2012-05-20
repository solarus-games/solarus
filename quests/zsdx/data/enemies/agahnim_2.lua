-- Agahnim (Boss of dungeon 8)

-- possible positions where he appears
local positions = {
  {x = 488, y = 317},
  {x = 560, y = 317},
  {x = 704, y = 317},
  {x = 776, y = 317},
  {x = 488, y = 421},
  {x = 560, y = 421},
  {x = 704, y = 421},
  {x = 776, y = 421},
  {x = 488, y = 525},
  {x = 560, y = 525},
  {x = 704, y = 525},
  {x = 776, y = 525},
}

local nb_sons_created = 0
local initial_life = 16
local blue_fireball_proba = 33 -- percent
local next_fireball_sound
local next_fireball_breed
local vulnerable = false
local hurt_proba
local middle_dialog = false
local nb_fakes_created = 0
local timers = {}

function event_appear()

  sol.enemy.set_life(initial_life)
  sol.enemy.set_damage(16)
  sol.enemy.create_sprite("enemies/agahnim_2")
  sol.enemy.set_optimization_distance(0)
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
  sol.enemy.set_position(-100, -100)
  sol.enemy.set_invincible()
  sol.enemy.set_attack_consequence("sword", "protected")
  sol.enemy.set_attack_consequence("arrow", "protected")
  sol.enemy.set_attack_consequence("hookshot", "protected")
  sol.enemy.set_attack_consequence("boomerang", "protected")
  sol.enemy.set_pushed_back_when_hurt(false)
  sol.enemy.set_push_hero_on_sword(true)
  sol.enemy.set_can_attack(false)

  local sprite = sol.enemy.get_sprite()
  sprite:set_animation("stopped")
end

function event_restart()

  vulnerable = false
  for _, t in ipairs(timers) do t:stop() end
  local sprite = sol.enemy.get_sprite()
  sprite:set_ignore_suspend(false)
  sprite:fade(1)
  timers[#timers + 1] = sol.main:start_timer(700, hide)
end

function event_update()

  -- look in the direction of the hero
  local sprite = sol.enemy.get_sprite()
  sprite:set_direction(get_direction4_to_hero())
end

function get_direction4_to_hero()

  local x, y = sol.enemy.get_position()
  local hero_x, hero_y = sol.map.hero_get_position()
  local angle = sol.main.get_angle(x, y, hero_x, hero_y)
  local direction4 = (angle + (math.pi / 4)) * 2 / math.pi
  return (math.floor(direction4) + 4) % 4
end

function hide()

  -- disappear for a while
  vulnerable = false
  sol.enemy.set_position(-100, -100)
  timers[#timers + 1] = sol.main:start_timer(500, unhide)
end

function unhide()

  -- come back somewhere
  local position = (positions[math.random(#positions)])
  sol.enemy.set_position(position.x, position.y)
  local sprite = sol.enemy.get_sprite()
  sprite:set_direction(get_direction4_to_hero())
  sprite:fade(0)
  timers[#timers + 1] = sol.main:start_timer(1000, fire_step_1)
end

function fire_step_1()

  -- before preparing a fireball
  local sprite = sol.enemy.get_sprite()
  sprite:set_animation("arms_up")
  timers[#timers + 1] = sol.main:start_timer(1000, fire_step_2)
  sol.enemy.set_can_attack(true)
end

function fire_step_2()

  -- prepare a fireball (red or blue)
  local sprite = sol.enemy.get_sprite()
  local blue = math.random(100) <= blue_fireball_proba

  if math.random(5) == 1 then
    -- don't tell the player if it will be red or blue
    sprite:set_animation("preparing_unknown_fireball")
  elseif blue then
    -- blue fireball: for the hero can do nothing but run away
    sprite:set_animation("preparing_blue_fireball")
  else
    -- red fireball: possible to throw it back to Agahnim
    sprite:set_animation("preparing_red_fireball")
  end

  if blue then
    next_fireball_sound = "cane"
    next_fireball_breed = "blue_fireball_triple"
  else
    next_fireball_sound = "boss_fireball"
    next_fireball_breed = "red_fireball_triple"
  end
  sol.audio.play_sound("boss_charge")
  timers[#timers + 1] = sol.main:start_timer(1500, fire_step_3)
end

function fire_step_3()

  -- throw the fireball(s)
  local sprite = sol.enemy.get_sprite()
  sprite:set_animation("stopped")
  sol.audio.play_sound(next_fireball_sound)
  vulnerable = true

  local delay -- delay before fading out and going somewhere else
  if next_fireball_breed == "blue_fireball_triple" then
    delay = 700
  else
    delay = 3000 -- red fireball: stay longer to play ping-pong
  end
  timers[#timers + 1] = sol.main:start_timer(delay, sol.enemy.restart)

  function throw_fire()
    nb_sons_created = nb_sons_created + 1
    sol.enemy.create_son("agahnim_fireball_" .. nb_sons_created,
        next_fireball_breed, 0, -21)
  end

  throw_fire()

  -- shoot more fireballs if the life becomes short
  local life = sol.enemy.get_life()
  if life <= initial_life / 2 then
    timers[#timers + 1] = sol.main:start_timer(200, throw_fire)
    timers[#timers + 1] = sol.main:start_timer(400, throw_fire)
    if life <= initial_life / 4 then
      timers[#timers + 1] = sol.main:start_timer(600, throw_fire)
      timers[#timers + 1] = sol.main:start_timer(800, throw_fire)
    end
  end

  -- play ping pong
  if life <= initial_life * 3 / 4 then
    hurt_proba = 20
  else
    hurt_proba = 100
  end
end

function event_message_received(src_enemy, message)

  if string.find(src_enemy, "^agahnim_fireball")
      and vulnerable then

    -- receive a fireball: get hurt or throw it back
    if math.random(100) <= hurt_proba then
      for _, t in ipairs(timers) do t:stop() end
      sol.map.enemy_remove(src_enemy)
      sol.enemy.hurt(1)
    else
      -- play ping-pong
      sol.audio.play_sound("boss_fireball")
      sol.enemy.send_message(src_enemy, "bounce")
      hurt_proba = hurt_proba + 20
    end
  end
end

function event_hurt(attack, life_lost)

  local sprite = sol.enemy.get_sprite()
  local life = sol.enemy.get_life()
  if life <= 0 then
    -- dying
    sol.map.enemy_remove_group("agahnim_fireball")
    sol.map.enemy_remove_group(sol.enemy.get_name() .. "_")
    sprite:set_ignore_suspend(true)
    sol.map.dialog_start("dungeon_8.agahnim_end")
    for _, t in ipairs(timers) do t:stop() end
  elseif life <= initial_life * 2 / 3 then
    -- not dying yet: start creating fakes after a few hits
    sprite:set_ignore_suspend(true)
    if not middle_dialog then
      sol.map.dialog_start("dungeon_8.agahnim_middle")
      middle_dialog = true
    end
    create_fakes()
  end
end

-- create fake Agahnims
function create_fakes()

  local prefix = sol.enemy.get_name() .. "_fake_"
  if sol.map.enemy_get_group_count(prefix) < 3 then
    nb_fakes_created = nb_fakes_created + 1
    local fake_name = prefix .. nb_fakes_created
    sol.enemy.create_son(fake_name, "agahnim_2_fake", 0, 0)
  end

  if sol.enemy.get_life() < initial_life / 3
      and sol.map.enemy_get_group_count(prefix) < 2 then
    -- create a second one
    create_fakes()
  end
end

