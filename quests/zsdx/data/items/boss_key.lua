function event_variant_changed(variant)
  -- the possession state of the boss key determines the built-in ability "open_dungeon_boss_lock"
  sol.game.set_ability("open_dungeon_boss_lock", variant)
end
