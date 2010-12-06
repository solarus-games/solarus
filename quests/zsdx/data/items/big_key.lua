function event_variant_changed(variant)
  -- the possession state of the big key determines the built-in ability "open_dungeon_big_locks"
  sol.game.set_ability("open_dungeon_big_locks", variant)
end
