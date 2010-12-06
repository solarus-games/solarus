function event_obtained(variant, savegame_variable)
  -- obtaining the boss key gives the built-in ability "open_dungeon_boss_lock"
  sol.game.set_ability("open_dungeon_boss_lock", 1)
end
