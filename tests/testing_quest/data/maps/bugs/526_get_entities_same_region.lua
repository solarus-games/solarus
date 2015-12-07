local map = ...

function map:on_started()

  -- First, build expected results according to names of entities.
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

    print("Test entity " .. sensor:get_name())

    local region_prefix = sensor:get_name():match("^(region_%a_)")
    assert(region_prefix ~= nil)

    local expected_in_region = sensors_by_region[region_prefix]
    assert(expected_in_region ~= nil)

    local found_in_region = {}
    for found in map:get_entities_in_region(sensor) do
      if found:get_name() ~= nil and found:get_name():match("^region_") then
        found_in_region[found] = true
      end
    end
    -- The sensor itself should not have been returned.
    assert(not found_in_region[sensor])

    -- Compare both sets.
    for expected in pairs(expected_in_region) do
      if expected ~= sensor then
        print("  Expected " .. expected:get_name())
        assert(found_in_region[expected])
        print("    Found it!")
      end
    end
    for found in pairs(found_in_region) do
      print("  Found " .. found:get_name())
      assert(expected_in_region[found])
      print("    Expected it!")
    end

    -- Do it again using coordinates.
    found_in_region = {}
    for found in map:get_entities_in_region(sensor:get_position()) do
      if found:get_name() ~= nil and found:get_name():match("^region_") then
        found_in_region[found] = true
      end
    end

    -- Compare both sets.
    for expected in pairs(expected_in_region) do
      print("  Expected " .. expected:get_name())
      assert(found_in_region[expected])
      print("    Found it!")
    end
    for found in pairs(found_in_region) do
      print("  Found " .. found:get_name())
      assert(expected_in_region[found])
      print("    Expected it!")
    end
  end

  sol.main.exit()
end
