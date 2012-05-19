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
local timers = {}

function event_appear()

  sol.enemy.set_life(1)
  sol.enemy.set_damage(8)
  sol.enemy.create_sprite("enemies/agahnim_2_fake")
  sol.enemy.set_optimization_distance(0)
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
  sprite:set_animation("stopped")

end

function event_restart()

  if not disappearing then
    sol.timer.stop(timers)
    local sprite = sol.enemy.get_sprite()
    sprite:fade(1)
    timers[#timers + 1] = sol.main:start_timer(500, hide)
  end
end

function event_update()

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

  sol.enemy.set_position(-100, -100)
  timers[#timers + 1] = sol.main:start_timer(500, unhide)
end

function unhide()

  local position = (positions[math.random(#positions)])
  sol.enemy.set_position(position.x, position.y)
  local sprite = sol.enemy.get_sprite()
  sprite:set_direction(get_direction4_to_hero())
  sprite:fade(0)
  timers[#timers + 1] = sol.main:start_timer(1000, fire_step_1)
end

function fire_step_1()

  local sprite = sol.enemy.get_sprite()
  sprite:set_animation("arms_up")
  timers[#timers + 1] = sol.main:start_timer(1000, fire_step_2)
  sol.enemy.set_can_attack(true)
end

function fire_step_2()

  local sprite = sol.enemy.get_sprite()
  local blue = math.random(100) <= blue_fireball_proba

  if math.random(5) == 1 then
    sprite:set_animation("preparing_unknown_fireball")
  elseif blue then
    sprite:set_animation("preparing_blue_fireball")
  else
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

  local sprite = sol.enemy.get_sprite()
  sprite:set_animation("stopped")
  sol.audio.play_sound(next_fireball_sound)
  timers[#timers + 1] = sol.main:start_timer(700, sol.enemy.restart)

  function throw_fire()

    nb_sons_created = nb_sons_created + 1
    sol.enemy.create_son(sol.enemy.get_name() .. "_fireball_" .. nb_sons_created,
        next_fireball_breed, 0, -21)
  end

  throw_fire()
  local life = sol.enemy.get_life()
end

function event_custom_attack_received(attack, sprite)

  sol.audio.play_sound("enemy_hurt")
  disappear()
end

function disappear()

  local sprite = sol.enemy.get_sprite()
  disappearing = true
  sol.enemy.set_can_attack(false)
  sprite:fade(1)
  sol.timer.stop(timers)
  timers[#timers + 1] = sol.main:start_timer(500, function()
    sol.map.enemy_remove(sol.enemy.get_name())
  end)
end

function event_collision_enemy(other_name, other_sprite, my_sprite)

  if not other_name:find("fireball") then

    local x = sol.enemy.get_position()
    if x > 0 then
      -- collision with another Agahnim
      sol.timer.stop(timers)
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

