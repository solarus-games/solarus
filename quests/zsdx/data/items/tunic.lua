local item = ...

function item:on_variant_changed(variant)
  -- the possession state of the tunic determines the built-in ability "tunic"
  self:get_game():set_ability("tunic", variant)
end

