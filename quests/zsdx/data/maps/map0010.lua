-- Outside world B1

function event_map_started(destination_point_name)

  -- enable dark world
  if sol.game.savegame_get_boolean(905) then
    sol.main.play_music("dark_mountain.spc")
    sol.map.tileset_set(13)
  end
end


