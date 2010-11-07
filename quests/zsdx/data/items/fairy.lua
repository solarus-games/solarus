function event_appear(variant)
  sol.main.create_movement("m", "random_movement")
  sol.main.movement_set_property("m", "speed", 32)
  sol.main.movement_set_property("m", "max_distance", 40)
  sol.item.set_movement("m")
end

