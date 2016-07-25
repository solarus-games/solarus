local map = ...
local game = map:get_game()

function map:on_opening_transition_finished()

  -- Check that the flying enemy did not fall.
  assert(enemy:exists())
  assert(enemy:get_life() > 0)
  sol.main.exit()
end
