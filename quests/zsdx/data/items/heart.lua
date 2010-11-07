trajectory = "0 0  0 -2  0 -2  0 -2  0 -2  0 -2  0 0  0 0  1 1  1 1  1 0  1 1  1 1  0 0  -1 0  -1 1  -1 0  -1 1  -1 0  -1 1  0 1  1 1  1 1  -1 0"

function event_appear(variant)
  sol.main.create_movement("m", "pixel_movement")
  sol.main.movement_set_property("m", "trajectory", trajectory)
  sol.main.movement_set_property("m", "delay", 100)
  sol.main.movement_set_property("m", "loop", false)
  sol.main.movement_set_property("m", "ignore_obstacles", false)
  sol.item.set_movement("m")
end

