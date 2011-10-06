-- Agahnim (Boss of dungeon 5)

-- possible positions where he appears
positions = {
  {x = 192, y = 277, direction4 = 0},
  {x = 400, y = 277, direction4 = 2},
  {x = 296, y = 373, direction4 = 1}
}

nb_sons_created = 0
initial_life = 8
finished = false

function event_appear()

  sol.enemy.set_life(initial_life)
  sol.enemy.set_damage(2)
  sol.enemy.create_sprite("enemies/agahnim")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
  sol.enemy.set_invincible()
  sol.enemy.set_attack_consequence("sword", "protected")
  sol.enemy.set_pushed_back_when_hurt(false)

  local sprite = sol.enemy.get_sprite()
  sol.main.sprite_set_animation(sprite, "stopped")
end

function event_restart()

  vulnerable = false
  sol.main.timer_stop_all()
  local sprite = sol.enemy.get_sprite()

  if not finished then
    sol.main.sprite_fade(sprite, 1)
    sol.main.timer_start(500, "hide")
  else
    sol.main.sprite_set_animation(sprite, "hurt")
    sol.map.hero_freeze()
    sol.main.timer_start(500, "end_dialog")
    sol.main.timer_start(1000, "fade_out")
    sol.main.timer_start(1500, "escape")
  end
end

function hide()

  vulnerable = false
  sol.enemy.set_position(-100, -100)
  sol.main.timer_start(500, "unhide")
end

function unhide()

  local position = (positions[math.random(3)])
  sol.enemy.set_position(position.x, position.y)
  local sprite = sol.enemy.get_sprite()
  sol.main.sprite_set_direction(sprite, position.direction4)
  sol.main.sprite_fade(sprite, 0)
  sol.main.timer_start(1000, "fire_step_1")
end

function fire_step_1()

  local sprite = sol.enemy.get_sprite()
  sol.main.sprite_set_animation(sprite, "arms_up")
  sol.main.timer_start(1000, "fire_step_2")
end

function fire_step_2()

  local sprite = sol.enemy.get_sprite()
  sol.main.sprite_set_animation(sprite, "preparing_fire")
  sol.main.play_sound("boss_charge")
  sol.main.timer_start(1500, "fire_step_3")
end

function fire_step_3()

  local sprite = sol.enemy.get_sprite()
  sol.main.sprite_set_animation(sprite, "stopped")
  sol.main.play_sound("boss_fireball")

  throw_fire()
  if sol.enemy.get_life() <= initial_life / 2 then
    sol.main.timer_start(200, "throw_fire")
    sol.main.timer_start(400, "throw_fire")
  end

  vulnerable = true
  sol.main.timer_start(700, "sol.enemy.restart")
end

function throw_fire()

  nb_sons_created = nb_sons_created + 1
  sol.enemy.create_son("agahnim_fireball_"..nb_sons_created, "fireball", 0, -21)
end

function event_message_received(src_enemy, message)

  if string.find(src_enemy, "^agahnim_fireball")
      and vulnerable then

    sol.main.timer_stop_all()
    sol.enemy.hurt(1)
    sol.map.enemy_remove(src_enemy)
  end
end

function event_hurt(attack, life_lost)

  if sol.enemy.get_life() <= 0 then

    sol.enemy.set_life(1)
    finished = true
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

