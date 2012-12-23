local item = ...

function item:on_created()

  self:set_savegame_variable("i1128")
end

function item:on_obtained(variant, savegame_variable)
  -- Give the built-in ability "tunic", but only after the treasure sequence is done.
  self:get_game():set_ability("tunic", variant)
end

