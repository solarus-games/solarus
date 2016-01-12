local map = ...

function map:on_started()

  wall:set_enabled(false)
  assert(not wall:is_enabled())
  assert(not hero:test_obstacles(0, 0))

  wall:set_enabled(true)
  assert(wall:is_enabled())
  assert(hero:test_obstacles(0, 0))

  sol.main.exit()
end
