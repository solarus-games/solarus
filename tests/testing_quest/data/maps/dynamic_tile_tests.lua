-- Tests for dynamic tile entities.

local map = ...

local function check_dynamic_tile(dynamic_tile, pattern_id, ground)

  local actual_pattern_id = dynamic_tile:get_pattern_id()
  local actual_ground = dynamic_tile:get_modified_ground()

  assert(pattern_id == actual_pattern_id)
  assert(ground == actual_ground)
end

function map:on_started()

  -- Test the dynamic tile API.
  check_dynamic_tile(dynamic_tile_47, "47", "wall")
  check_dynamic_tile(dynamic_tile_67, "67", "traversable")

  sol.main.exit()
end
