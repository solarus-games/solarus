-- Poutroxor: an evil giant bird from Newlink
-- Phase 1: a skeleton is on the bird and has to be killed with arrows.
-- Phase 2: once the skeleton is dead, the bird moves faster, throws blue flames and can be killed by shooting arrows in his head.

local bird, head, skeleton -- sprites

function event_appear()

  sol.enemy.set_life(10)
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

  local m = sol.main.random_movement_create(32)
  sol.enemy.start_movement(m)
  sol.main.timer_stop_all()
  sol.main.timer_start(skeleton_attack, math.random(5000))
end

-- Make the skeleton throw a blue flame
function skeleton_attack()


end

