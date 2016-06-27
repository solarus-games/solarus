local map = ...
local game = map:get_game()

-- Starting a map in a hole:
-- The hero should fall only after the opening transition.

function map:on_started()

  assert(hero:get_state() == "free")
end

function map:on_opening_transition_finished()
  assert(hero:get_state() == "falling")
end
