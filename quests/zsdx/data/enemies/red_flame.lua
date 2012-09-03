local enemy = ...

-- A red flame shot by another enemy.
-- When its flame finishes its movement,
-- a circle of fire remains on the ground.

function enemy:on_created()

  self:set_life(1)
  self:set_damage(1)
  self:create_sprite("enemies/red_flame")
  self:set_size(16, 16)
  self:set_origin(8, 13)
  self:set_invincible()
  self:set_obstacle_behavior("flying")
  self:set_layer_independent_collisions(true)
  self:set_optimization_distance(0)
end

function enemy:on_restarted()

  -- By default, go to where the hero is now.
  local x, y = self:get_map():get_entity("hero"):get_position()
  x = x + math.random(-32, 32)
  y = y + math.random(-32, 32)
  local m = sol.movement.create("target")
  m:set_speed(144)
  m:set_target(x, y)
  m:set_ignore_obstacles(true)
  self:start_movement(m)
  sol.timer.start(self, 2000, function() self:on_movement_finished() end)
end

function enemy:on_movement_finished(movement)

  -- Let a circle of fire on the ground.
  local son_name = self:get_name() .. "_circle"
  self:create_enemy(son_name, "fire_circle", 0, 0)
  self:remove()
end

function enemy:go(angle)

  local m = sol.movement.create("straight")
  m:set_speed(144)
  m:set_angle(angle)
  m:set_ignore_obstacles(true)
  m:set_max_distance(320)
  self:start_movement(m)
end

