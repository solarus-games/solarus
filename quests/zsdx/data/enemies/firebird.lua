-- Firebird

local main_sprite = nil
local claw_sprite = nil
local nb_sons_created = 0
local initial_life = 7

function event_appear()

  sol.enemy.set_life(initial_life)
  sol.enemy.set_damage(2)
  main_sprite = sol.enemy.create_sprite("enemies/firebird")
  claw_sprite = sol.enemy.create_sprite("enemies/firebird")
  sol.main.sprite_set_animation(claw_sprite, "claw")
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

  sol.main.sprite_set_animation(claw_sprite, "claw")
  local m = sol.main.random_movement_create(64)
  sol.enemy.start_movement(m)
  sol.main.timer_stop_all()
  sol.main.timer_start(prepare_flames, math.random(2000, 6000))
end

function prepare_flames()

  local hero_x, hero_y = sol.map.hero_get_position()
  local prefix = sol.enemy.get_name() .. "_son_"
  nb_sons_created = nb_sons_created + 1
  local life_lost = initial_life - sol.enemy.get_life()
  local nb_to_create = 3 + life_lost

  function repeat_throw_flame()
    sol.main.play_sound("lamp")
    local son_name = prefix .. nb_sons_created
    sol.enemy.create_son(son_name, "red_flame", 0, -16, 0)
    nb_to_create = nb_to_create - 1
    if nb_to_create > 0 then
      sol.main.timer_start(repeat_throw_flame, 200)
    end
  end
  repeat_throw_flame()

  sol.main.timer_start(prepare_flames, math.random(4000, 6000))
end

function event_hurt(attack, life_lost)

  sol.main.timer_stop_all()
  if sol.enemy.get_life() <= 0 then
    sol.map.enemy_remove_group(sol.enemy.get_name() .. "_")
  end
end

