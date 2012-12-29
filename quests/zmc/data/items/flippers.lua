local item = ...

function item:on_created()

  self:set_savegame_variable("i1118")
end

function item:on_variant_changed(variant)
  -- the possession state of the flippers determines the built-in ability "swim"
  self:get_game():set_ability("swim", variant)
end

