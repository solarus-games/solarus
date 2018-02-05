local map = ...
local game = map:get_game()

function map:on_started()

  -- Verify existing properties.
  assert_equal(sensor_1:get_property("my_prop"), "val")
  assert_equal(sensor_1:get_property("other"), "1")
  assert_equal(sensor_1:get_property("this_one_does_not_exist"), nil)
  assert_equal(sensor_1:get_property("a_new_one"), nil)

  -- Add a property.
  sensor_1:set_property("a_new_one", "hello")
  assert_equal(sensor_1:get_property("a_new_one"), "hello")

  -- Change a property.
  assert_equal(sensor_1:get_property("my_prop"), "val")
  sensor_1:set_property("my_prop", "new_val")
  assert_equal(sensor_1:get_property("my_prop"), "new_val")

  -- Remove a property.
  assert_equal(sensor_1:get_property("other"), "1")
  sensor_1:set_property("other", nil)
  assert_equal(sensor_1:get_property("other"), nil)

  sol.main.exit()
end
