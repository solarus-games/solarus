local item = ...

function item:on_created()

  self:set_savegame_variable("i1129")
  self:set_sound_when_picked(nil)
  self:set_sound_when_brandished(nil)
end

function item:on_variant_changed(variant)
  -- The possession state of the sword determines the built-in ability "sword".
  self:get_game():set_ability("sword", variant)
end

