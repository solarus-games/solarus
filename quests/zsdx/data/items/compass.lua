function event_variant_changed(variant)
  -- the possession state of the compass determines the built-in ability "see_dungeon_minimap_elements"
  sol.game.set_ability("see_dungeon_minimap_elements", variant)
end
