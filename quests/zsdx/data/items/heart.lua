trajectory = "0 0  0 -2  0 -2  0 -2  0 -2  0 -2  0 0  0 0  1 1  1 1  1 0  1 1  1 1  0 0  -1 0  -1 1  -1 0  -1 1  -1 0  -1 1  0 1  1 1  1 1  -1 0"

function event_appear(variant)
  sol.main.create_movement("heart_movement", "pixel_movement")
  sol.main.movement_set_property("heart_movement", "trajectory", trajectory)
  sol.main.movement_set_property("heart_movement", "delay", 100)
  sol.main.movement_set_property("heart_movement", "loop", false)
  sol.main.movement_set_property("heart_movement", "ignore_obstacles", false)
  sol.item.set_movement("heart_movement")
end

