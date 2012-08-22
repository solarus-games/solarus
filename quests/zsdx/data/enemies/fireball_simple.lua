local enemy = ...

-- A simple fireball thrown by another enemy.

function enemy:on_created()

  self:set_life(1)
  self:set_damage(12)
  self:create_sprite("enemies/fireball_simple")
  self:set_size(16, 16)
  self:set_origin(8, 8)
  self:set_invincible()
  self:set_obstacle_behavior("flying")
end

function enemy:on_obstacle_reached()

  self:remove()
end

function enemy:go(angle)

  local m = sol.movement.create("straight")
  m:set_speed(192)
  m:set_angle(angle)
  self:start_movement(m)
end

