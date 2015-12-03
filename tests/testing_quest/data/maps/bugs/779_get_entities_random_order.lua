-- Checks that map:get_entities() gives entities in Z order.

local map = ...

function map:on_started()

  local new_names = { "entity_a" , "entity_b", "entity_c", "entity_d", "aaa" }
  for _, name in ipairs(new_names) do
    map:create_sensor({
      name = name,
      x = "64",
      y = "61",
      layer = 0,
      width = 16,
      height = 16,
    })
  end

  -- Check that map:get_entities() gives names in Z order.
  local expected_names = { "destination", "entity_a", "entity_b", "entity_c", "entity_d", "aaa" }
  local i = 1
  for entity in map:get_entities("") do

    if entity:get_type() ~= "hero" and
        entity:get_type() ~= "camera" then  -- Skip built-ins.

      assert(entity:get_name() ~= nil)
      print("Name: " .. entity:get_name())
      assert(entity:get_name() == expected_names[i])
      i = i + 1
    end
  end
  assert(i == #expected_names + 1)

  sol.main.exit()
end
