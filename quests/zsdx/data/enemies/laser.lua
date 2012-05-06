-- Laser ray

local initial_position = {}

function event_appear()

  sol.enemy.set_life(1)
  sol.enemy.set_damage(4)
  sol.enemy.create_sprite("enemies/laser")
  sol.enemy.set_optimization_distance(0)
  sol.enemy.set_size(8, 8)
  sol.enemy.set_origin(4, 4)
  sol.enemy.set_can_hurt_hero_running(true)
  sol.enemy.set_invincible()
  sol.enemy.set_minimum_shield_needed(3)
  sol.enemy.set_obstacle_behavior("flying")

  initial_position.x, initial_position.y, initial_position.layer = sol.enemy.get_position()
end

function event_restart()

  local sprite = sol.enemy.get_sprite()
  local direction4 = sprite:get_direction()
  local m = sol.movement.create("path")
  m:set_path{direction4 * 2}
  m:set_speed(300)
  m:set_loop(true)
  sol.enemy.start_movement(m)
end

function event_obstacle_reached()

  sol.enemy.set_position(initial_position.x, initial_position.y, initial_position.layer)
end

