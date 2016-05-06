local map = ...

local detected_2 = false
local detected_3 = false

custom_entity_2:add_collision_test("overlapping", function(first_entity, second_entity)
  assert(second_entity == custom_entity)
  detected_2 = true
end)

custom_entity_3:add_collision_test("overlapping", function(first_entity, second_entity)
  assert(second_entity == custom_entity)
  detected_3 = true
end)

function map:on_opening_transition_finished()

  local x, y = custom_entity:get_position()
  local origin_x, origin_y = custom_entity:get_origin()
  assert(x == 24 and y == 61)
  assert(origin_x == 8 and origin_y == 13)
  assert(not custom_entity:overlaps(custom_entity_2))

  custom_entity:set_origin(0, 0)
  assert(detected_2)

  x, y = custom_entity:get_position()
  origin_x, origin_y = custom_entity:get_origin()
  assert(x == 24 and y == 61)
  assert(origin_x == 0 and origin_y == 0)
  assert(custom_entity:overlaps(custom_entity_2))

  custom_entity:set_size(32, 32)
  assert(detected_3)

  x, y = custom_entity:get_position()
  origin_x, origin_y = custom_entity:get_origin()
  local width, height = custom_entity:get_size()
  assert(x == 24 and y == 61)
  assert(origin_x == 0 and origin_y == 0)
  assert(width == 32 and height == 32)
  assert(custom_entity:overlaps(custom_entity_3))

  sol.main.exit()

end
