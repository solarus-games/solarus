local map = ...

function map:on_opening_transition_finished()

  sol.timer.start(30, function()
    local hero_x, hero_y = hero:get_position()
    local destination_x, destination_y = destination_2:get_position()
    assert(hero_x == destination_x)
    assert(hero_y == destination_y)
    sol.main.exit()
  end)
end
