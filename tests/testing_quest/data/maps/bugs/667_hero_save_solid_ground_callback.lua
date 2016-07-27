local map = ...

hole_1:set_modified_ground("hole")
hole_1:set_enabled(false)
hole_2:set_modified_ground("hole")
hole_2:set_enabled(false)
hole_3:set_modified_ground("hole")
hole_3:set_enabled(false)

local function check_position(entity, expected_x, expected_y, expected_layer)

  local x, y, layer = entity:get_position()
  assert_equal(x, expected_x)
  assert_equal(y, expected_y)
  assert_equal(layer, expected_layer)
end

local function check_solid_ground(entity, expected_x, expected_y, expected_layer)

  local x, y, layer = entity:get_solid_ground_position()
  assert_equal(x, expected_x)
  assert_equal(y, expected_y)
  assert_equal(layer, expected_layer)
end

function map:on_started()

  hero:save_solid_ground(hole_2:get_position())
end

function map:on_opening_transition_finished()

  check_position(hero, destination:get_position())
  check_solid_ground(hero, hole_2:get_position())
  hole_1:set_enabled(true)
  print("Hole 1 enabled")

  sol.timer.start(2000, function()
    check_position(hero, hole_2:get_position())
    hero:save_solid_ground(function()
      return hole_3:get_position()
    end)
    check_solid_ground(hero, hole_3:get_position())
    hole_2:set_enabled(true)
    print("Hole 2 enabled")
    sol.timer.start(2000, function()
      check_position(hero, hole_3:get_position())
      check_solid_ground(hero, hole_3:get_position())

      hero:save_solid_ground(destination:get_position())
      check_solid_ground(hero, destination:get_position())
      hero:reset_solid_ground()
      check_solid_ground(hero, hole_3:get_position())

      hole_3:set_enabled(true)

      sol.timer.start(2000, sol.main.exit)
    end)
  end)

end
