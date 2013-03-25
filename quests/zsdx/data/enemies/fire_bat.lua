local enemy = ...

-- A fire bat shot by Ganon.

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

  self:remove()
end

-- Goes towards the hero.
function enemy:go_hero()

  local angle = self:get_angle(self:get_map():get_entity("hero"))
  local m = sol.movement.create("straight")
  m:set_speed(192)
  m:set_angle(angle)
  m:set_max_distance(320)
  m:set_ignore_obstacles(true)
  m:start(self)
end

-- Makes circles around the boss.
function enemy:go_circle(center_entity)

  local m = sol.movement.create("circle")
  m:set_center(center_entity)
  m:set_radius(48)
  m:set_center_dy(-21)
  m:set_initial_angle(90)
  m:set_angle_speed(150)
  m:start(self)
end

