local map = ...

local ground_below_from_event

function map:on_started()

  hole:set_modified_ground("hole")
  assert(hole:get_modified_ground() == "hole")
end

function map:on_opening_transition_finished()

  assert(map:get_ground(ground_observer:get_position()) == "traversable")
  assert(ground_below_from_event == "traversable")

  hole:set_position(ground_observer:get_position())
  assert(hole:is_enabled())
  assert(hole:get_modified_ground() == "hole")
  assert(map:get_ground(ground_observer:get_position()) == "hole")
  assert(ground_below_from_event == "hole")

  sol.main.exit()
end

function ground_observer:on_ground_below_changed(ground)

  print("Ground changed: " .. ground)
  ground_below_from_event = ground
end
