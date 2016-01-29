local map = ...

water:set_modified_ground("deep_water")
raft:set_modified_ground("traversable")
raft:set_can_traverse_ground("deep_water", true)
raft:set_can_traverse_ground("traversable", false)

function map:on_started()

  assert(raft:can_traverse_ground("deep_water"))
  assert(not raft:can_traverse_ground("traversable"))
  assert(map:get_ground(raft:get_position()) == "traversable")
  assert(not raft:test_obstacles(0, 0))

  sol.main.exit()
end
