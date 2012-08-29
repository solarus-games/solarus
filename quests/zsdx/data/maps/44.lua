local map = ...

-- Enemy testing arena.

local init_evil_tiles = sol.main.load_file("maps/evil_tiles")
init_evil_tiles(map)

function map:on_started(destination)

  map:set_entities_enabled("evil_tile_", false)
  sol.timer.start(2000, function()
    map:start_evil_tiles()
  end)
end

