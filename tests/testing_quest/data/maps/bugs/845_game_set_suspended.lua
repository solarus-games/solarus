local map = ...
local game = map:get_game()

function map:on_started()

  -- Suspended during the opening transition.
  assert(game:is_suspended())

  -- Suspended by both the opening transition and the script.
  game:set_suspended(true)
  assert(game:is_suspended())

  -- Still suspended by the opening transition.
  game:set_suspended(false)
  assert(game:is_suspended())
end

function map:on_opening_transition_finished()

  assert(not game:is_suspended())

  game:set_suspended(true)
  assert(game:is_suspended())

  game:set_suspended(false)
  assert(not game:is_suspended())

  sol.main.exit()
end
