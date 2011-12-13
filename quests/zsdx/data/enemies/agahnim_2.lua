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

function event_appear()

  sol.enemy.set_life(initial_life)
  sol.enemy.set_damage(16)
  sol.enemy.create_sprite("enemies/agahnim_2")
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
  sol.main.sprite_set_animation(sprite, "stopped")
end

function event_restart()

  vulnerable = false
  sol.main.timer_stop_all()
  local sprite = sol.enemy.get_sprite()
  sol.main.sprite_set_animation_ignore_suspend(sprite, false)
  sol.main.sprite_fade(sprite, 1)
  sol.main.timer_start(hide, 700)
end

function event_update()

  -- look in the direction of the hero
  local sprite = sol.enemy.get_sprite()
  sol.main.sprite_set_direction(sprite, get_direction4_to_hero())
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
  sol.main.timer_start(unhide, 500)
end

function unhide()

  -- come back somewhere
  local position = (positions[math.random(#positions)])
  sol.enemy.set_position(position.x, position.y)
  local sprite = sol.enemy.get_sprite()
  sol.main.sprite_set_direction(sprite, get_direction4_to_hero())
  sol.main.sprite_fade(sprite, 0)
  sol.main.timer_start(fire_step_1, 1000)
end

function fire_step_1()

  -- before preparing a fireball
  local sprite = sol.enemy.get_sprite()
  sol.main.sprite_set_animation(sprite, "arms_up")
  sol.main.timer_start(fire_step_2, 1000)
  sol.enemy.set_can_attack(true)
end

function fire_step_2()

  -- prepare a fireball (red or blue)
  local sprite = sol.enemy.get_sprite()
  local blue = math.random(100) <= blue_fireball_proba

  if math.random(5) == 1 then
    -- don't tell the player if it will be red or blue
    sol.main.sprite_set_animation(sprite, "preparing_unknown_fireball")
  elseif blue then
    -- blue fireball: for the hero can do nothing but run away
    sol.main.sprite_set_animation(sprite, "preparing_blue_fireball")
  else
    -- red fireball: possible to throw it back to Agahnim
    sol.main.sprite_set_animation(sprite, "preparing_red_fireball")
  end

  if blue then
    next_fireball_sound = "cane"
    next_fireball_breed = "blue_fireball_triple"
  else
    next_fireball_sound = "boss_fireball"
    next_fireball_breed = "red_fireball_triple"
  end
  sol.main.play_sound("boss_charge")
  sol.main.timer_start(fire_step_3, 1500)
end

function fire_step_3()

  -- throw the fireball(s)
  local sprite = sol.enemy.get_sprite()
  sol.main.sprite_set_animation(sprite, "stopped")
  sol.main.play_sound(next_fireball_sound)
  vulnerable = true

  local delay -- delay before fading out and going somewhere else
  if next_fireball_breed == "blue_fireball_triple" then
    delay = 700
  else
    delay = 3000 -- red fireball: stay longer to play ping-pong
  end
  sol.main.timer_start(sol.enemy.restart, delay)

  function throw_fire()
    nb_sons_created = nb_sons_created + 1
    sol.enemy.create_son("agahnim_fireball_" .. nb_sons_created,
        next_fireball_breed, 0, -21)
  end

  throw_fire()

  -- shoot more fireballs if the life becomes short
  local life = sol.enemy.get_life()
  if life <= initial_life / 2 then
    sol.main.timer_start(throw_fire, 200)
    sol.main.timer_start(throw_fire, 400)
    if life <= initial_life / 4 then
      sol.main.timer_start(throw_fire, 600)
      sol.main.timer_start(throw_fire, 800)
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
      sol.main.timer_stop_all()
      sol.map.enemy_remove(src_enemy)
      sol.enemy.hurt(1)
    else
      -- play ping-pong
      sol.main.play_sound("boss_fireball")
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
    sol.main.sprite_set_animation_ignore_suspend(sprite, true)
    sol.map.dialog_start("dungeon_8.agahnim_end")
    sol.main.timer_stop_all()
  elseif life <= initial_life * 2 / 3 then
    -- not dying yet: start creating fakes after a few hits
    sol.main.sprite_set_animation_ignore_suspend(sprite, true)
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

