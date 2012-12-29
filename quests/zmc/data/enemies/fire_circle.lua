local enemy = ...

-- A fixed trace of fire in circle on the ground.

function enemy:on_created()

  self:set_life(1)
  self:set_damage(1)
  self:create_sprite("enemies/fire_circle")
  self:set_size(16, 16)
  self:set_origin(8, 13)
  self:set_invincible()
end

function enemy:on_restarted()

  -- Disappear after a few seconds.
  sol.timer.start(self, 2000, function()
    self:remove()
  end)
end

