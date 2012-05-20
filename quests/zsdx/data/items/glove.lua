function event_variant_changed(variant)
  -- the possession state of the glove determines the built-in ability "lift"
  sol.map.get_game():set_ability("lift", variant)
end

