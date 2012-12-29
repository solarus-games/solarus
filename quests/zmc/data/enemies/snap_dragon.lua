local enemy = ...

-- Snap Dragon.

local timer

function enemy:on_created()

  self:set_life(3)
  self:set_damage(4)
  self:create_sprite("enemies/snap_dragon")
  self:set_hurt_style("normal")
  self:set_size(16, 16)
  self:set_origin(8, 13)
end

function enemy:on_restarted()

  self:go_random()
end

function enemy:on_movement_finished(movement)

  self:go_random()
end

function enemy:on_obstacle_reached(movement)

  self:go_random()
end

function enemy:on_hurt(attack, life_lost)

  if timer ~= nil then
    timer:stop()
    timer = nil
  end
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
  self:start_movement(m)

  local sprite = self:get_sprite()
  sprite:set_direction(rand4 - 1)

  if timer ~= nil then
    timer:stop()
  end
  timer = sol.timer.start(self, 300 + math.random(1500), function()
    sprite:set_animation("bite")
  end)
end

function enemy:on_sprite_animation_finished(sprite, animation)

  if animation == "bite" then
    sprite:set_animation("walking")
  end
end

