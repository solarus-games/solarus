local map = ...

local function check_traversable()
  assert(not hero:test_obstacles(0, 0))
end

local function check_obstacle()
  assert(hero:test_obstacles(0, 0))
end

function map:on_started()

  check_traversable()

  for destination in map:get_entities("traversable_destination_") do
    print("Checking " .. destination:get_name())
    hero:set_position(destination:get_position())
    check_traversable()
  end

  for destination in map:get_entities("obstacle_destination_") do
    print("Checking " .. destination:get_name())
    hero:set_position(destination:get_position())
    check_obstacle()
  end

  sol.timer.start(10, sol.main.exit)
end
