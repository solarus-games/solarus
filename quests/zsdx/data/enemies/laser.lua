-- Laser ray

local initial_position = {}

function event_appear()

  sol.enemy.set_life(1)
  sol.enemy.set_damage(4)
  sol.enemy.create_sprite("enemies/laser")
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
  local direction4 = sol.main.sprite_get_direction(sprite)
  local m = sol.main.path_movement_create(tostring(direction4 * 2), 300)
  sol.main.movement_set_property(m, "loop", true)
  sol.enemy.start_movement(m)
end

function event_obstacle_reached()

  sol.enemy.set_position(initial_position.x, initial_position.y, initial_position.layer)
end

