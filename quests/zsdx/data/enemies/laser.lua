local enemy = ...

-- Laser ray.

local initial_position = {}

function enemy:on_created()

  self:set_life(1)
  self:set_damage(4)
  self:create_sprite("enemies/laser")
  self:set_optimization_distance(0)
  self:set_size(8, 8)
  self:set_origin(4, 4)
  self:set_can_hurt_hero_running(true)
  self:set_invincible()
  self:set_minimum_shield_needed(3)
  self:set_obstacle_behavior("flying")

  initial_position.x, initial_position.y, initial_position.layer = self:get_position()
end

function enemy:on_restarted()

  local sprite = self:get_sprite()
  local direction4 = sprite:get_direction()
  local m = sol.movement.create("path")
  m:set_path{direction4 * 2}
  m:set_speed(300)
  m:set_loop(true)
  self:start_movement(m)
end

function enemy:on_obstacle_reached()

  self:set_position(initial_position.x, initial_position.y, initial_position.layer)
end

