local item = ...

function item:on_appear(variant, savegame_variable, falling_height)

  if falling_height ~= 0 then
    local trajectory = {
      { 0,  0},
      { 0, -2},
      { 0, -2},
      { 0, -2},
      { 0, -2},
      { 0, -2},
      { 0,  0},
      { 0,  0},
      { 1,  1},
      { 1,  1},
      { 1,  0},
      { 1,  1},
      { 1,  1},
      { 0,  0},
      {-1,  0},
      {-1,  1},
      {-1,  0},
      {-1,  1},
      {-1,  0},
      {-1,  1},
      { 0,  1},
      { 1,  1},
      { 1,  1},
      {-1,  0}
    }
    local m = sol.movement.create("pixel")
    m:set_trajectory(trajectory)
    m:set_delay(100)
    m:set_loop(false)
    m:set_ignore_obstacles(true)
    self:start_movement(m)
  end
end

