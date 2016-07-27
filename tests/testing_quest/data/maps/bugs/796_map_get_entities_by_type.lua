local map = ...

local function get_num_entities_type(type)

  local num_entities = 0
  for entity in map:get_entities_by_type(type) do
    num_entities = num_entities + 1
  end
  return num_entities
end

function map:on_started()

  local expected = {
    hero = 1,
    camera = 1,
    destination = 2,
    sensor = 1,
    tile = 0,
    dynamic_tile = 0,
  }

  for type, expected_count in pairs(expected) do
    print("Type " .. type)
    local count = get_num_entities_type(type)
    assert(count == expected_count)
  end

  sol.main.exit()
end
