function event_variant_changed(variant)
  -- the possession state of the map determines the built-in ability "see_dungeon_minimap_rooms"
  sol.game.set_ability("see_dungeon_minimap_rooms", variant)
end
