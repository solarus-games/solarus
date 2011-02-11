function event_variant_changed(variant)
  -- the possession state of the sword determines the built-in ability "sword"
  sol.game.set_ability("sword", variant)
end
