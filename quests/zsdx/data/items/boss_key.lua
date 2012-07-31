local item = ...

function item:on_obtained(variant, savegame_variable)
  -- obtaining the boss key gives the built-in ability "open_dungeon_boss_locks"
  self:get_game():set_ability("open_dungeon_boss_locks", 1)
end

