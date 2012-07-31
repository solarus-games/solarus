local item = ...

function item:on_obtained(variant, savegame_variable)
  -- obtaining the big key gives the built-in ability "open_dungeon_big_locks"
  item:get_game():set_ability("open_dungeon_big_locks", 1)
end

