
function event_appear(variant, savegame_variable, falling_height)

  if falling_height ~= 0 then
    local trajectory = "0 0  0 -2  0 -2  0 -2  0 -2  0 -2  0 0  0 0  1 1  1 1  1 0  1 1  1 1  0 0  -1 0  -1 1  -1 0  -1 1  -1 0  -1 1  0 1  1 1  1 1  -1 0"
    local m = sol.main.pixel_movement_create(trajectory, 100)
    sol.main.movement_set_property(m, "loop", false)
    sol.main.movement_set_property(m, "ignore_obstacles", true)
    sol.item.start_movement(m)
  end
end

