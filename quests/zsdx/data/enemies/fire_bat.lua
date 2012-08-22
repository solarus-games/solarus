local enemy = ...

-- A fire bat thrown by Ganon

function enemy:on_created()

  self:set_life(1)
  self:set_damage(12)
  self:create_sprite("enemies/fire_bat")
  self:set_size(16, 16)
  self:set_origin(8, 8)
  self:set_invincible()
  self:set_attack_consequence("sword", 1)
  self:set_attack_consequence("arrow", 1)
  self:set_attack_consequence("hookshot", 1)
  self:set_attack_consequence("boomerang", 1)
  self:set_obstacle_behavior("flying")
  self:set_layer_independent_collisions(true)
end

function enemy:on_restarted()

  if self:get_movement() == nil then
    self:go_hero()
  end
end

function enemy:on_movement_finished(movement)

  self:get_map():enemy_remove(self:get_name())
end

function enemy:go_hero()

  local x, y = self:get_position()
  local hero_x, hero_y = self:get_map():hero_get_position()
  local angle = sol.main.get_angle(x, y, hero_x, hero_y)
  local m = sol.movement.create("straight")
  m:set_speed(192)
  m:set_angle(angle)
  m:set_max_distance(320)
  m:set_ignore_obstacles(true)
  self:start_movement(m)
end

function enemy:go_circle()

  -- make a circle around the father
  local m = sol.movement.create("circle")
  m:set_center(7, self:get_father())
  m:set_radius(48)
  m:set_center_dy(-21)
  m:set_initial_angle(90)
  m:set_angle_speed(150)
  self:start_movement(m)
end

function enemy:on_message_received(src_enemy, message)

  if message == "circle" then
    self:go_circle()
  elseif message == "go_hero" then
    self:go_hero()
  else
    local delay = tonumber(message:match("^go_hero ([0-9]*)$"))
    if delay then
      sol.timer.start(delay, function()
	self:go_hero()
      end)
    end
  end
end

