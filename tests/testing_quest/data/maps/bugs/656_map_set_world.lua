local map = ...

function map:on_started()

  assert(map:get_world() == nil)
  map:set_world("world_656")
  assert(map:get_world() == "world_656")
  map:set_world(nil)
  assert(map:get_world() == nil)

  assert(map:get_floor() == nil)
  map:set_floor(5)
  assert(map:get_floor() == 5)
  map:set_floor(nil)
  assert(map:get_floor() == nil)

  sol.main.exit()
end
