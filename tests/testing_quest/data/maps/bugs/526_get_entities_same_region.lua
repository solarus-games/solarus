local map = ...

function map:on_started()

  -- First get all entities and build expected results according to their name.
  local empty = true
  local sensors_by_region = {}
  for sensor in map:get_entities("region_") do
    local region_prefix = sensor:get_name():match("^(region_%a_)")
    assert(region_prefix ~= nil)
    if sensors_by_region[region_prefix] == nil then
      sensors_by_region[region_prefix] = {}
    end
    local in_region = sensors_by_region[region_prefix]
    in_region[sensor] = true
    empty = false
  end
  assert(not empty)

  -- For each entity, check that get_entities_in_same_region()
  -- returns exactly the expected ones.
  for sensor in map:get_entities("region_") do

    local region_prefix = sensor:get_name():match("^(region_%a_)")
    assert(region_prefix ~= nil)

    local expected_in_region = sensors_by_region[region_prefix]
    assert(expected_in_region ~= nil)

    local found_in_region = {}
    for sensor_found in map:get_entities_in_same_region(sensor) do
      found_in_region[sensor_found] = true
    end

    -- Compare both sets.
    for expected in expected_in_region do
      assert(found_in_region[expected])
    end
    for found in found_in_region do
      assert(expected_in_region[found])
    end
  end

  sol.main.exit()
end
