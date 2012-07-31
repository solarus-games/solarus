local item = ...

function item:on_obtained(variant, savegame_variable)
  -- obtaining the map gives the built-in ability "see_dungeon_minimap_rooms"
  self:get_game():set_ability("see_dungeon_minimap_elements", 1)
end

