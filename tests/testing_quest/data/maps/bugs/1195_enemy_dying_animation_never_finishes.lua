local map = ...
local game = map:get_game()

function map:on_opening_transition_finished()

  enemy:set_life(0)

  sol.timer.start(map, 1000, function()
    assert(enemy == nil)
    sol.main.exit()
  end)
end
