local item = ...

function item:on_created()

  self:set_savegame_variable("i1126")
  self:set_amount_savegame_variable("i1029")
  self:set_max_amount(3)
end

