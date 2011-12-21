function event_variant_changed(variant)
  -- the possession state of the world map determines the built-in ability "see_outside_world_minimap"
  sol.game.set_ability("see_outside_world_minimap", variant)
end

