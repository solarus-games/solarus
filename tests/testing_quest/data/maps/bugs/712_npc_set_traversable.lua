local map = ...

assert(not usual_npc_1:is_traversable())
assert(not usual_npc_2:is_traversable())
usual_npc_2:set_traversable(true)
assert(usual_npc_2:is_traversable())

assert(not generalized_npc_1:is_traversable())
assert(not generalized_npc_2:is_traversable())
generalized_npc_2:set_traversable(true)
assert(generalized_npc_2:is_traversable())

function map:on_started()

  hero:set_position(usual_npc_1:get_position())
  assert(hero:test_obstacles())

  hero:set_position(usual_npc_2:get_position())
  assert(not hero:test_obstacles())

  hero:set_position(generalized_npc_1:get_position())
  assert(hero:test_obstacles())

  hero:set_position(generalized_npc_2:get_position())
  assert(not hero:test_obstacles())

  sol.main.exit()
end
