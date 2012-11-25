local item = ...

function item:on_created()

  self:set_savegame_variable("i1130")
end

function item:on_variant_changed(variant)
  -- The possession state of the shield determines the built-in ability "shield".
  self:get_game():set_ability("shield", variant)
end

