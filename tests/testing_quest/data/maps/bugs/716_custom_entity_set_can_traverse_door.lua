local map = ...

custom_entity:set_can_traverse("door", true)

function map:on_started()

  assert(not custom_entity:test_obstacles())
  sol.main.exit()
end
