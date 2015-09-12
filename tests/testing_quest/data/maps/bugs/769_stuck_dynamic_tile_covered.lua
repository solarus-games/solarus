local map = ...

function map:on_started()

  -- The hero is on a traversable dynamic tile that covers
  -- a non-traversable dynamic tile.
  -- The traversable one should win, like with normal tiles.
  assert(not hero:test_obstacles(0, 0))
end

sol.timer.start(10, sol.main.exit)
