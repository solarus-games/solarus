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

function event_appear()

  sol.enemy.set_life(initial_life)
  sol.enemy.set_damage(12)
  sol.enemy.create_sprite("enemies/agahnim")
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
  sol.main.sprite_set_animation(sprite, "stopped")
end

function event_restart()

  vulnerable = false
  sol.main.timer_stop_all()
  local sprite = sol.enemy.get_sprite()

  if not finished then
    sol.main.sprite_fade(sprite, 1)
    sol.main.timer_start(hide, 700)
  else
    sol.main.sprite_set_animation(sprite, "hurt")
    sol.map.hero_freeze()
    sol.main.timer_start(end_dialog, 500)
    sol.main.timer_start(fade_out, 1000)
    sol.main.timer_start(escape, 1500)
  end
end

function hide()

  vulnerable = false
  sol.enemy.set_position(-100, -100)
  sol.main.timer_start(unhide, 500)
end

function unhide()

  local position = (positions[math.random(#positions)])
  sol.enemy.set_position(position.x, position.y)
  local sprite = sol.enemy.get_sprite()
  sol.main.sprite_set_direction(sprite, position.direction4)
  sol.main.sprite_fade(sprite, 0)
  sol.main.timer_start(fire_step_1, 1000)
end

function fire_step_1()

  local sprite = sol.enemy.get_sprite()
  sol.main.sprite_set_animation(sprite, "arms_up")
  sol.main.timer_start(fire_step_2, 1000)
end

function fire_step_2()

  local sprite = sol.enemy.get_sprite()
  if math.random(100) <= blue_fireball_proba then
    sol.main.sprite_set_animation(sprite, "preparing_blue_fireball")
  else
    sol.main.sprite_set_animation(sprite, "preparing_red_fireball")
  end
  sol.main.play_sound("boss_charge")
  sol.main.timer_start(fire_step_3, 1500)
end

function fire_step_3()

  local sprite = sol.enemy.get_sprite()

  local sound, breed
  if sol.main.sprite_get_animation(sprite) == "preparing_blue_fireball" then
    sound = "cane"
    breed = "blue_fireball_triple"
  else
    sound = "boss_fireball"
    breed = "red_fireball_triple"
  end
  sol.main.sprite_set_animation(sprite, "stopped")
  sol.main.play_sound(sound)

  vulnerable = true
  sol.main.timer_start(sol.enemy.restart, 700)

  function throw_fire()

    nb_sons_created = nb_sons_created + 1
    sol.enemy.create_son("agahnim_fireball_" .. nb_sons_created,
    breed, 0, -21)
  end

  throw_fire()
  if sol.enemy.get_life() <= initial_life / 2 then
    sol.main.timer_start(throw_fire, 200)
    sol.main.timer_start(throw_fire, 400)
  end
end

function event_message_received(src_enemy, message)

  if string.find(src_enemy, "^agahnim_fireball")
      and vulnerable then

    sol.main.timer_stop_all()
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
  sol.main.sprite_set_animation_ignore_suspend(sprite, true)
  sol.map.dialog_start("dungeon_5.agahnim_end")
end

function fade_out()

  local sprite = sol.enemy.get_sprite()
  sol.main.sprite_fade(sprite, 1)
end

function escape()

  local x, y = sol.enemy.get_position()
  sol.map.pickable_item_create("heart_container", 1, 521, x, y, 0)
  sol.map.hero_unfreeze()
  sol.map.enemy_remove(sol.enemy.get_name())
  sol.game.savegame_set_boolean(520, true)
end

