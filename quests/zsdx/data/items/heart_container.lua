function event_obtained(variant, savegame_variable)

  sol.map.get_game():add_max_life(4)
  sol.map.get_game():add_life(sol.map.get_game():get_max_life())
end

