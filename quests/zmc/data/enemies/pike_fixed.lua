local enemy = ...

-- Pike that does not move.

function enemy:on_created()

  self:set_life(1)
  self:set_damage(4)
  self:create_sprite("enemies/pike_fixed")
  self:set_size(16, 16)
  self:set_origin(8, 13)
  self:set_can_hurt_hero_running(true)
  self:set_invincible()
end

