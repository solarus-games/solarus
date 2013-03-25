local enemy = ...

-- Tentacle: a basic enemy that follows the hero.

function enemy:on_created()

  self:set_life(1)
  self:set_damage(2)
  self:create_sprite("enemies/tentacle")
  self:set_size(16, 16)
  self:set_origin(8, 13)
end

function enemy:on_restarted()

  local m = sol.movement.create("path_finding")
  m:set_speed(32)
  m:start(self)
end

