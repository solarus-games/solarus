local map = ...
local game = map:get_game()

local function check_property(actual_key, actual_value, expected_key, expected_value)
  assert_equal(actual_key, expected_key)
  assert_equal(actual_value, expected_value)
end

function map:on_started()

  -- Verify existing properties.
  assert_equal(sensor_1:get_property("my_prop"), "val")
  assert_equal(sensor_1:get_property("other"), "1")
  assert_equal(sensor_1:get_property("this_one_does_not_exist"), nil)
  assert_equal(sensor_1:get_property("a_new_one"), nil)

  -- Add a property.
  sensor_1:set_property("a_new_one", "hello")
  assert_equal(sensor_1:get_property("a_new_one"), "hello")

  -- Change a property to an empty string.
  assert_equal(sensor_1:get_property("my_prop"), "val")
  sensor_1:set_property("my_prop", "")
  assert_equal(sensor_1:get_property("my_prop"), "")

  -- Change a property to a non-empty string.
  sensor_1:set_property("my_prop", "new_val")
  assert_equal(sensor_1:get_property("my_prop"), "new_val")

  -- Remove a property.
  assert_equal(sensor_1:get_property("other"), "1")
  sensor_1:set_property("other", nil)
  assert_equal(sensor_1:get_property("other"), nil)

  sol.main.exit()
end
