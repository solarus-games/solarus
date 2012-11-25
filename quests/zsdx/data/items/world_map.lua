local item = ...

function item:on_created()

  self:set_savegame_variable("i1031")
end

function item:on_variant_changed(variant)
  -- The possession state of the world map determines the built-in ability "see_outside_world_minimap".
  self:get_game():set_ability("see_outside_world_minimap", variant)
end

