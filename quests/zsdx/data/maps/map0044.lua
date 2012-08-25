local map = ...

local init_evil_tiles = sol.main.load_file("maps/evil_tiles")
init_evil_tiles(map)

-- Enemy testing arena.

function map:on_started(destination_point_name)

  sol.timer.start(2000, function()
    map:start_evil_tiles()
  end)
end

