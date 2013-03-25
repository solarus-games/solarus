local enemy = ...

-- A blue flame shot by another enemy.

function enemy:on_created()

  self:set_life(1)
  self:set_damage(8)
  self:create_sprite("enemies/blue_flame")
  self:set_size(16, 16)
  self:set_origin(8, 13)
  self:set_invincible()
  self:set_obstacle_behavior("flying")
  self:set_layer_independent_collisions(true)
  self:set_optimization_distance(0)
end

function enemy:on_movement_finished(movement)

  self:remove()
end

function enemy:go(angle)

  local m = sol.movement.create("straight")
  m:set_speed(192)
  m:set_angle(angle)
  m:set_ignore_obstacles(true)
  m:set_max_distance(320)
  m:start(self)
end

