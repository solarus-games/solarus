local map = ...

local function check_sprites_box(entity, expected_x, expected_y, expected_width, expected_height)

  print("Checking sprites box of " .. entity:get_type())

  local x, y, width, height = entity:get_sprites_bounding_box()
  assert(x == expected_x)
  assert(y == expected_y)
  assert(width == expected_width)
  assert(height == expected_height)
end


function map:on_started()

  -- Check an entity without sprite.
  check_sprites_box(wall, wall:get_bounding_box())

  -- Check an entity with a sprite of the same bounding box.
  check_sprites_box(block, block:get_bounding_box())

  -- Check an entity with larger sprites.
  check_sprites_box(hero, -4, -8, 56, 56)

  sol.main.exit()
end
