local item = ...

function item:on_created()

  self:set_savegame_variable("i1119")
end

function item:on_variant_changed(variant)
  -- The possession state of the magic_cape determines the built-in ability
  -- "detect_weak_walls".
  self:get_game():set_ability("detect_weak_walls", variant)
end

