function event_variant_changed(variant)
  -- the possession state of the tunic determines the built-in ability "tunic"
  sol.game.set_ability("tunic", variant)
end

