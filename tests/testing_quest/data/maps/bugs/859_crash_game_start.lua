local map = ...
local game = map:get_game()

function map:on_started()

  if game.restarted then
    sol.main.exit()
  end
end

function map:on_opening_transition_finished()

  if not game.restarted then
    game.restarted = true
    game:start()
  end
end
