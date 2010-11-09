trajectory = "0 0  0 -2  0 -2  0 -2  0 -2  0 -2  0 0  0 0  1 1  1 1  1 0  1 1  1 1  0 0  -1 0  -1 1  -1 0  -1 1  -1 0  -1 1  0 1  1 1  1 1  -1 0"

function event_appear(variant)
  m = sol.main.pixel_movement_create(trajectory, 100)
  sol.main.movement_set_property(m, "loop", false)
  sol.main.movement_set_property(m, "ignore_obstacles", false)
  sol.item.set_movement(m)
end

