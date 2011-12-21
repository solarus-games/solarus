function event_variant_changed(variant)
  -- the possession state of the shield determines the built-in ability "shield"
  sol.game.set_ability("shield", variant)
end

