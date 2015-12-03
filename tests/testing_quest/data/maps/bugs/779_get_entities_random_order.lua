local map = ...

function map:on_started()

  local new_names = { "entity_a", "entity_b" , "entity_c", "entity_d", "entity_e" }
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
  local names = { "destination", "hero", "entity_a", "entity_b" , "entity_c", "entity_d", "entity_e" }
  local i = 1
  for entity in map:get_entities("") do
    print("Name: " .. entity:get_name() or "")
    assert(entity:get_name() == names[i])
    i = i + 1
  end
end
