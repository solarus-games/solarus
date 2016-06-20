local map = ...

function map:on_started()

  local x, y = hero:get_position()

  sol.timer.start(10000, function()
    local new_x, new_y = hero:get_position()
    assert(new_x == x and new_y == y)
    sol.main.exit()
  end)
end
