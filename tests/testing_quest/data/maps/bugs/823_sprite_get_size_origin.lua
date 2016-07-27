local map = ...

function map:on_started()

  local sprite = block:get_sprite()
  local width, height = sprite:get_size()
  local origin_x, origin_y = sprite:get_origin()
  assert(width == 16)
  assert(height == 16)
  assert(origin_x == 8)
  assert(origin_y == 13)

  sol.main.exit()
end
