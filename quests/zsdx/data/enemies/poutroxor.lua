-- Poutroxor: an evil giant bird from Newlink
-- Phase 1: a skeleton is on the bird and has to be killed with arrows.
-- Phase 2: once the skeleton is dead, the bird moves faster, throws blue flames and can be killed by shooting arrows in his head.

local bird, head, skeleton -- sprites
local phase = 1
local nb_flames_created = 0
local max_flames_created = 10

function event_appear()

  sol.enemy.set_life(14)
  sol.enemy.set_damage(1)
  bird = sol.enemy.create_sprite("enemies/poutroxor")
  head = sol.enemy.create_sprite("enemies/poutroxor_head")
  skeleton = sol.enemy.create_sprite("enemies/poutroxor_skeleton")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 8)
  sol.enemy.set_obstacle_behavior("flying")
  sol.enemy.set_pushed_back_when_hurt(false)
  sol.enemy.set_push_hero_on_sword(true)

  sol.enemy.set_invincible()
  sol.enemy.set_attack_consequence("sword", "protected")
  sol.enemy.set_attack_consequence("hookshot", "protected")
  sol.enemy.set_attack_consequence("boomerang", "protected")
  sol.enemy.set_attack_consequence("arrow", "ignored")
  sol.enemy.set_attack_consequence_sprite(skeleton, "arrow", 1)
end

function event_restart()

  if phase == 1 then
    local m = sol.main.random_movement_create(32)
    sol.enemy.start_movement(m)
    sol.main.timer_stop_all()
    sol.main.timer_start(skeleton_attack, math.random(2000, 3000))
  else
    local m = sol.main.random_movement_create(80)
    sol.enemy.start_movement(m)
    sol.main.timer_stop_all()
    sol.main.timer_start(big_attack, math.random(3000, 5000))
  end
end

-- Make the skeleton throw a blue flame
function skeleton_attack()

  sol.main.sprite_set_animation(skeleton, "attack")
  sol.main.play_sound("ice")
  sol.main.timer_start(function()
    sol.main.sprite_set_animation(skeleton, "walking")
    nb_flames_created = nb_flames_created + 1
    local son_name = sol.enemy.get_name() .. "_son_" .. nb_flames_created
    sol.enemy.create_son(son_name, "blue_flame", 0, -48, 2)
    local x, y = sol.map.enemy_get_position(son_name)
    local hero_x, hero_y = sol.map.hero_get_position()
    local angle = sol.main.get_angle(x, y, hero_x, hero_y)
    sol.enemy.send_message(son_name, angle)
    sol.main.timer_start(skeleton_attack, math.random(1000, 3000))
  end, 500)
end

function event_hurt(attack, life_lost)

  sol.main.timer_stop_all()
  if phase == 1 and sol.enemy.get_life() - life_lost <= 7 then
    sol.enemy.stop_movement()
    sol.main.play_sound("enemy_killed")
    sol.main.sprite_set_animation(skeleton, "hurt")
    phase = 2
    sol.enemy.remove_sprite(skeleton)
    sol.map.enemy_remove_group(sol.enemy.get_name() .. "_son")
    skeleton = nil
    sol.enemy.set_attack_consequence_sprite(head, "arrow", 1)
  elseif sol.enemy.get_life() - life_lost <= 0 then
    sol.map.enemy_remove_group(sol.enemy.get_name() .. "_son")
  end
end

-- Makes the bird throw several blue flames
function big_attack()

  nb_flames_created = 0
  sol.enemy.stop_movement()
  sol.main.sprite_set_animation(head, "attack")
  sol.main.play_sound("lamp")
  sol.main.timer_start(repeat_flame, 500)
end

function repeat_flame()

  if nb_flames_created <= max_flames_created then
    local son_name = sol.enemy.get_name() .. "_son_" .. nb_flames_created
    local angle = math.random(30, 90) * math.pi / 40
    nb_flames_created = nb_flames_created + 1
    sol.enemy.create_son(son_name, "blue_flame", 0, 16)
    sol.enemy.send_message(son_name, tostring(angle))
    sol.main.play_sound("lamp")
    sol.main.timer_start(repeat_flame, 150)
  else
    sol.main.timer_start(sol.enemy.restart, 500)
  end
end

