-- Marmite of the witch (custom entity).
local entity = ...

function entity:on_created()
  self:set_size(24, 16)
  self:set_origin(12, 13)
  self:set_traversable_by(false)
end