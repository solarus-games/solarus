local map = ...

function map:on_started()

  local tested_entity = map:create_custom_entity({
    layer = 0,
    x = 160,
    y = 117,
    width = 16,
    height = 16,
    direction = 0,
  })

  local ground_entity = map:create_custom_entity({
    layer = 0,
    x = 160,
    y = 117,
    width = 16,
    height = 16,
    direction = 0,
  })

  local grounds = {
    "traversable",
    "wall",
    "low_wall",
    "deep_water",
    "shallow_water",
    "grass",
    "hole",
    "ice",
    "ladder",
    "prickles",
    "lava"
  }

  for _, ground in ipairs(grounds) do
    print("Testing ground: " .. ground)
    ground_entity:set_modified_ground(ground)
  
    tested_entity:set_can_traverse_ground(ground, true)
    assert(not tested_entity:test_obstacles(0, 0))

    tested_entity:set_can_traverse_ground(ground, false)
    assert(tested_entity:test_obstacles(0, 0))
  end

  sol.main.exit()
end
