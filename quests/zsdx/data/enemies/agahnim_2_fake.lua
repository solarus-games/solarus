-- Fake Agahnim (with the boss of dungeon 8)

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
local blue_fireball_proba = 33 -- percent
local next_fireball_sound
local next_fireball_breed
local disappearing = false

function event_appear()

  sol.enemy.set_life(1)
  sol.enemy.set_damage(8)
  sol.enemy.create_sprite("enemies/agahnim_2_fake")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
  sol.enemy.set_position(-100, -100)
  sol.enemy.set_invincible()
  sol.enemy.set_attack_consequence("sword", "custom")
  sol.enemy.set_attack_consequence("arrow", "custom")
  sol.enemy.set_attack_consequence("hookshot", "custom")
  sol.enemy.set_attack_consequence("boomerang", "custom")
  sol.enemy.set_pushed_back_when_hurt(false)
  sol.enemy.set_push_hero_on_sword(true)
  sol.enemy.set_can_attack(false)

  local sprite = sol.enemy.get_sprite()
  sol.main.sprite_set_animation(sprite, "stopped")

end

function event_restart()

  if not disappearing then
    sol.main.timer_stop_all()
    local sprite = sol.enemy.get_sprite()
    sol.main.sprite_fade(sprite, 1)
    sol.main.timer_start(hide, 500)
  end
end

function event_update()

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

  sol.enemy.set_position(-100, -100)
  sol.main.timer_start(unhide, 500)
end

function unhide()

  local position = (positions[math.random(#positions)])
  sol.enemy.set_position(position.x, position.y)
  local sprite = sol.enemy.get_sprite()
  sol.main.sprite_set_direction(sprite, get_direction4_to_hero())
  sol.main.sprite_fade(sprite, 0)
  sol.main.timer_start(fire_step_1, 1000)
end

function fire_step_1()

  local sprite = sol.enemy.get_sprite()
  sol.main.sprite_set_animation(sprite, "arms_up")
  sol.main.timer_start(fire_step_2, 1000)
  sol.enemy.set_can_attack(true)
end

function fire_step_2()

  local sprite = sol.enemy.get_sprite()
  local blue = math.random(100) <= blue_fireball_proba

  if math.random(5) == 1 then
    sol.main.sprite_set_animation(sprite, "preparing_unknown_fireball")
  elseif blue then
    sol.main.sprite_set_animation(sprite, "preparing_blue_fireball")
  else
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

  local sprite = sol.enemy.get_sprite()
  sol.main.sprite_set_animation(sprite, "stopped")
  sol.main.play_sound(next_fireball_sound)
  sol.main.timer_start(sol.enemy.restart, 700)

  function throw_fire()

    nb_sons_created = nb_sons_created + 1
    sol.enemy.create_son(sol.enemy.get_name() .. "_fireball_" .. nb_sons_created,
        next_fireball_breed, 0, -21)
  end

  throw_fire()
  local life = sol.enemy.get_life()
end

function event_custom_attack_received(attack, sprite)

  sol.main.play_sound("enemy_hurt")
  disappear()
end

function disappear()

  local sprite = sol.enemy.get_sprite()
  disappearing = true
  sol.enemy.set_can_attack(false)
  sol.main.sprite_fade(sprite, 1)
  sol.main.timer_stop_all()
  sol.main.timer_start(function()
    sol.map.enemy_remove(sol.enemy.get_name())
  end, 500)
end

function event_collision_enemy(other_name, other_sprite, my_sprite)

  if not other_name:find("fireball") then

    local x = sol.enemy.get_position()
    if x > 0 then
      -- collision with another Agahnim
      sol.main.timer_stop_all()
      hide() -- go somewhere else
    end
  end
end

function event_message_received(src_enemy, message)

  if string.find(src_enemy, "^agahnim_fireball") then
    -- receive a fireball: disappear
    disappear()
  end
end

