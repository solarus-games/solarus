local enemy = ...

-- A red flame thrown by another enemy

function enemy:on_appear()

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

function enemy:on_restart()

  -- by default, go to where the hero is now
  local x, y = self:get_map():hero_get_position()
  x = x + math.random(-32, 32)
  y = y + math.random(-32, 32)
  local m = sol.movement.create("target")
  m:set_speed(144)
  m:set_target(x, y)
  m:set_ignore_obstacles(true)
  self:start_movement(m)
  sol.timer.start(2000, event_movement_finished)
end

function enemy:on_movement_finished(movement)

  -- create a circle of fire
  local son_name = self:get_name() .. "_circle"
  self:create_son(son_name, "fire_circle", 0, 0)
  self:get_map():enemy_remove(self:get_name())
end

function enemy:on_message_received(src_enemy, message)

  -- the message is the angle to take
  local angle = tonumber(message)
  local m = sol.movement.create("straight")
  m:set_speed(144)
  m:set_angle(angle)
  m:set_ignore_obstacles(true)
  m:set_max_distance(320)
  self:start_movement(m)
end

