local enemy = ...

-- Snap Dragon.

enemy:set_life(3)
enemy:set_damage(4)
enemy:set_hurt_style("normal")
enemy:set_size(16, 16)
enemy:set_origin(8, 13)
local sprite = enemy:create_sprite("enemies/snap_dragon")

function enemy:on_restarted()

  self:go_random()
end

function enemy:on_movement_finished(movement)

  self:go_random()
end

function enemy:on_obstacle_reached(movement)

  self:go_random()
end

function enemy:go_random()

  -- Random diagonal direction.
  local rand4 = math.random(4)
  local direction8 = rand4 * 2 - 1
  local angle = direction8 * math.pi / 4
  local m = sol.movement.create("straight")
  m:set_speed(48)
  m:set_angle(angle)
  m:set_max_distance(24 + math.random(96))
  m:start(self)

  sprite:set_direction(rand4 - 1)

  sol.timer.stop_all(self)
  sol.timer.start(self, 300 + math.random(1500), function()
    sprite:set_animation("bite")
  end)
end

function sprite:on_animation_finished(animation)

  if animation == "bite" then
    self:set_animation("walking")
  end
end

