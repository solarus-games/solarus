local map = ...

function map:on_started()

  assert(walkable_switch:is_walkable())
  assert(not solid_switch:is_walkable())

  sol.main.exit()
end
