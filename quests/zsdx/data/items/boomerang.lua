local item = ...

function item:on_use()

  if self:get_variant() == 1 then
    self:get_map():hero_start_boomerang(128, 160, "boomerang1", "entities/boomerang1")
  else
    -- boomerang 2: longer and faster movement
    self:get_map():hero_start_boomerang(192, 320, "boomerang2", "entities/boomerang2")
  end
  self:set_finished()
end

