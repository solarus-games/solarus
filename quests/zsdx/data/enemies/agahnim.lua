-- Agahnim (Boss of dungeon 5)

-- possible positions where he appears
local positions = {
  {x = 192, y = 277, direction4 = 3},
  {x = 400, y = 277, direction4 = 3},
  {x = 296, y = 373, direction4 = 1}
}

local nb_sons_created = 0
local initial_life = 10
local finished = false
local blue_fireball_proba = 33 -- percent
local timers = {}

function event_appear()

  sol.enemy.set_life(initial_life)
  sol.enemy.set_damage(12)
  sol.enemy.create_sprite("enemies/agahnim")
  sol.enemy.set_optimization_distance(0)
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
  sol.enemy.set_invincible()
  sol.enemy.set_attack_consequence("sword", "protected")
  sol.enemy.set_attack_consequence("arrow", "protected")
  sol.enemy.set_attack_consequence("hookshot", "protected")
  sol.enemy.set_attack_consequence("boomerang", "protected")
  sol.enemy.set_pushed_back_when_hurt(false)
  sol.enemy.set_push_hero_on_sword(true)

  local sprite = sol.enemy.get_sprite()
  sprite:set_animation("stopped")
end

function event_restart()

  vulnerable = false
  for _, t in ipairs(timers) do t:stop() end
  local sprite = sol.enemy.get_sprite()

  if not finished then
    sprite:fade(1)
    timers[#timers + 1] = sol.main:start_timer(700, hide)
  else
    sprite:set_animation("hurt")
    sol.map.hero_freeze()
    timers[#timers + 1] = sol.main:start_timer(500, end_dialog)
    timers[#timers + 1] = sol.main:start_timer(1000, fade_out)
    timers[#timers + 1] = sol.main:start_timer(1500, escape)
  end
end

function hide()

  vulnerable = false
  sol.enemy.set_position(-100, -100)
  timers[#timers + 1] = sol.main:start_timer(500, unhide)
end

function unhide()

  local position = (positions[math.random(#positions)])
  sol.enemy.set_position(position.x, position.y)
  local sprite = sol.enemy.get_sprite()
  sprite:set_direction(position.direction4)
  sprite:fade(0)
  timers[#timers + 1] = sol.main:start_timer(1000, fire_step_1)
end

function fire_step_1()

  local sprite = sol.enemy.get_sprite()
  sprite:set_animation("arms_up")
  timers[#timers + 1] = sol.main:start_timer(1000, fire_step_2)
end

function fire_step_2()

  local sprite = sol.enemy.get_sprite()
  if math.random(100) <= blue_fireball_proba then
    sprite:set_animation("preparing_blue_fireball")
  else
    sprite:set_animation("preparing_red_fireball")
  end
  sol.audio.play_sound("boss_charge")
  timers[#timers + 1] = sol.main:start_timer(1500, fire_step_3)
end

function fire_step_3()

  local sprite = sol.enemy.get_sprite()

  local sound, breed
  if sprite:get_animation() == "preparing_blue_fireball" then
    sound = "cane"
    breed = "blue_fireball_triple"
  else
    sound = "boss_fireball"
    breed = "red_fireball_triple"
  end
  sprite:set_animation("stopped")
  sol.audio.play_sound(sound)

  vulnerable = true
  timers[#timers + 1] = sol.main:start_timer(700, sol.enemy.restart)

  function throw_fire()

    nb_sons_created = nb_sons_created + 1
    sol.enemy.create_son("agahnim_fireball_" .. nb_sons_created,
    breed, 0, -21)
  end

  throw_fire()
  if sol.enemy.get_life() <= initial_life / 2 then
    timers[#timers + 1] = sol.main:start_timer(200, throw_fire)
    timers[#timers + 1] = sol.main:start_timer(400, throw_fire)
  end
end

function event_message_received(src_enemy, message)

  if string.find(src_enemy, "^agahnim_fireball")
      and vulnerable then

    for _, t in ipairs(timers) do t:stop() end
    sol.map.enemy_remove(src_enemy)
    sol.enemy.hurt(1)
  end
end

function event_hurt(attack, life_lost)

  local life = sol.enemy.get_life()
  if life <= 0 then
    sol.map.enemy_remove_group("agahnim_fireball")
    sol.enemy.set_life(1)
    finished = true
  elseif life <= initial_life / 3 then
    blue_fireball_proba = 50
  end
end

function end_dialog()

  for i = 1, nb_sons_created do
    son = "agahnim_fire_ball_"..i
    if not sol.map.enemy_is_dead(son) then
      sol.map.enemy_remove(son)
    end
  end

  local sprite = sol.enemy.get_sprite()
  sprite:set_ignore_suspend(true)
  sol.map.dialog_start("dungeon_5.agahnim_end")
end

function fade_out()

  local sprite = sol.enemy.get_sprite()
  sprite:fade(1)
end

function escape()

  local x, y = sol.enemy.get_position()
  sol.map.pickable_item_create("heart_container", 1, 521, x, y, 0)
  sol.map.hero_unfreeze()
  sol.map.enemy_remove(sol.enemy.get_name())
  sol.map.get_game():set_boolean(520, true)
end

