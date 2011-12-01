-- Agahnim (Boss of dungeon 8)

-- possible positions where he appears
positions = {
  {x = 488, y = 317, direction4 = 3},
  {x = 560, y = 317, direction4 = 3},
  {x = 704, y = 317, direction4 = 3},
  {x = 776, y = 317, direction4 = 3},
  {x = 488, y = 421, direction4 = 3},
  {x = 560, y = 421, direction4 = 3},
  {x = 704, y = 421, direction4 = 3},
  {x = 776, y = 421, direction4 = 3},
  {x = 488, y = 525, direction4 = 1},
  {x = 560, y = 525, direction4 = 1},
  {x = 704, y = 525, direction4 = 1},
  {x = 776, y = 525, direction4 = 1},
}

nb_sons_created = 0
initial_life = 8

function event_appear()

  sol.enemy.set_life(initial_life)
  sol.enemy.set_damage(4)
  sol.enemy.create_sprite("enemies/agahnim")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
  sol.enemy.set_invincible()
  sol.enemy.set_attack_consequence("sword", "protected")
  sol.enemy.set_pushed_back_when_hurt(false)
  sol.enemy.set_push_hero_on_sword(true)

  local sprite = sol.enemy.get_sprite()
  sol.main.sprite_set_animation(sprite, "stopped")
end

function event_restart()

  vulnerable = false
  sol.main.timer_stop_all()
  local sprite = sol.enemy.get_sprite()
  sol.main.sprite_fade(sprite, 1)
  sol.main.timer_start(hide, 500)
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
  sol.main.sprite_set_animation(sprite, "preparing_fire")
  sol.main.play_sound("boss_charge")
  sol.main.timer_start(fire_step_3, 1500)
end

function fire_step_3()

  local sprite = sol.enemy.get_sprite()
  sol.main.sprite_set_animation(sprite, "stopped")
  sol.main.play_sound("boss_fireball")

  throw_fire()
  if sol.enemy.get_life() <= initial_life / 2 then
    sol.main.timer_start(throw_fire, 200)
    sol.main.timer_start(throw_fire, 400)
  end

  vulnerable = true
  sol.main.timer_start(sol.enemy.restart, 700)
end

function throw_fire()

  nb_sons_created = nb_sons_created + 1
  sol.enemy.create_son("agahnim_fireball_"..nb_sons_created, "fireball_triple", 0, -21)
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

  if sol.enemy.get_life() <= 0 then
    sol.map.enemy_remove_group("agahnim_fireball")
    local sprite = sol.enemy.get_sprite()
    sol.main.sprite_set_animation_ignore_suspend(sprite, true)
    sol.map.dialog_start("dungeon_8.agahnim_end")
    sol.main.timer_stop_all()
  end
end

