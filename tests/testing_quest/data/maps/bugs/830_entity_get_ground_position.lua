local map = ...

function map:on_started()

  local hero_x, hero_y, hero_layer = hero:get_position()
  local hole = map:create_custom_entity({
    x = hero_x,
    y = hero_y,
    layer = hero_layer,
    direction = 0,
    width = 16,
    height = 16,
  })

  assert(map:get_ground(hero_x, hero_y, hero_layer) == "traversable")

  hole:set_modified_ground("hole")

  -- Hole overlapping both the hero's origin point and his ground point.
  local hero_ground_x, hero_ground_y, hero_ground_layer = hero:get_ground_position()
  assert(hero_ground_x == hero_x)
  assert(hero_ground_y == hero_y - 2)
  assert(hero_ground_layer == hero_layer)
  assert(map:get_ground(hero_x, hero_y, hero_layer) == "hole")
  assert(map:get_ground(hero_ground_x, hero_ground_y, hero_ground_layer) == "hole")
  assert(hero:get_ground_below() == "hole")

  -- Hole overlapping the hero's origin point but not his ground point.
  hole:set_position(hero_x, hero_y + 12)
  assert(map:get_ground(hero_x, hero_y, hero_layer) == "hole")
  assert(map:get_ground(hero_ground_x, hero_ground_y, hero_ground_layer) == "traversable")
  assert(hero:get_ground_below() == "traversable")

  -- Hole overlapping the hero's ground point but not his origin point.
  hole:set_position(hero_x, hero_y - 4)
  assert(map:get_ground(hero_x, hero_y, hero_layer) == "traversable")
  assert(map:get_ground(hero_ground_x, hero_ground_y, hero_ground_layer) == "hole")
  assert(hero:get_ground_below() == "hole")

  -- Also check an entity that is not a ground observer.
  assert(destination:get_ground_below() == "hole")

  sol.main.exit()
end
