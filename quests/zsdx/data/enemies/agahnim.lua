-- Agahnim (Boss of dungeon 5)

-- possible positions where he appears
positions = {
  {x = 192, y = 277, direction4 = 0},
  {x = 400, y = 277, direction4 = 2},
  {x = 296, y = 373, direction4 = 1}
}

nb_sons_created = 0
initial_life = 8

-- The enemy appears: set its properties
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
  sol.main.sprite_fade(sprite, 1)
  sol.main.timer_start(500, "hide")
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
    sol.main.timer_start(150, "throw_fire")
    sol.main.timer_start(300, "throw_fire")
  end

  vulnerable = false
  sol.main.timer_start(400, "set_vulnerable")
  sol.main.timer_start(700, "sol.enemy.restart")
end

function throw_fire()

  nb_sons_created = nb_sons_created + 1
  sol.enemy.create_son("agahnim_fireball_"..nb_sons_created, "fireball", 0, -21)
end

function set_vulnerable()
  vulnerable = true
end

function event_message_received(src_enemy, message)

  if string.find(src_enemy, "^agahnim_fireball")
      and vulnerable then

    sol.main.timer_stop_all()
    sol.enemy.hurt(1)
    sol.map.enemy_remove(src_enemy)
  end
end

