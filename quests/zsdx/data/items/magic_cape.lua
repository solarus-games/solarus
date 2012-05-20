function event_variant_changed(variant)
  -- the possession state of the magic_cape determines the built-in ability
  -- "detect_weak_walls"
  sol.map.get_game():set_ability("detect_weak_walls", variant)
end

