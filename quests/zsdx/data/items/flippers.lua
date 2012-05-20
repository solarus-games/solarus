function event_variant_changed(variant)
  -- the possession state of the flippers determines the built-in ability "swim"
  sol.map.get_game():set_ability("swim", variant)
end

