-- Firebird

local main_sprite = nil
local claw_sprite = nil
local nb_sons_created = 0
local initial_life = 7
local timers = {}

function event_appear()

  sol.enemy.set_life(initial_life)
  sol.enemy.set_damage(2)
  main_sprite = sol.enemy.create_sprite("enemies/firebird")
  claw_sprite = sol.enemy.create_sprite("enemies/firebird")
  claw_sprite:set_animation("claw")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
  sol.enemy.set_obstacle_behavior("flying")
  sol.enemy.set_layer_independent_collisions(true)

  sol.enemy.set_invincible()
  sol.enemy.set_attack_consequence("sword", "protected")
  sol.enemy.set_attack_consequence("hookshot", "protected")
  sol.enemy.set_attack_consequence("boomerang", "protected")
  sol.enemy.set_attack_consequence("arrow", "protected")
  sol.enemy.set_attack_consequence_sprite(claw_sprite, "arrow", 1)
  sol.enemy.set_pushed_back_when_hurt(false)
  sol.enemy.set_push_hero_on_sword(true)
end

function event_restart()

  claw_sprite:set_animation("claw")
  local m = sol.movement.create("random")
  m:set_speed(64)
  sol.enemy.start_movement(m)
  for _, t in ipairs(timers) do t:stop() end
  timers[#timers + 1] = sol.timer.start(math.random(2000, 6000), prepare_flames)
end

function prepare_flames()

  local hero_x, hero_y = sol.map.hero_get_position()
  local prefix = sol.enemy.get_name() .. "_son_"
  nb_sons_created = nb_sons_created + 1
  local life_lost = initial_life - sol.enemy.get_life()
  local nb_to_create = 3 + life_lost

  function repeat_throw_flame()
    sol.audio.play_sound("lamp")
    local son_name = prefix .. nb_sons_created
    sol.enemy.create_son(son_name, "red_flame", 0, -16, 0)
    nb_to_create = nb_to_create - 1
    if nb_to_create > 0 then
      timers[#timers + 1] = sol.timer.start(200, repeat_throw_flame)
    end
  end
  repeat_throw_flame()

  timers[#timers + 1] = sol.timer.start(math.random(4000, 6000), prepare_flames)
end

function event_hurt(attack, life_lost)

  for _, t in ipairs(timers) do t:stop() end
  if sol.enemy.get_life() <= 0 then
    sol.map.enemy_remove_group(sol.enemy.get_name() .. "_")
  end
end

