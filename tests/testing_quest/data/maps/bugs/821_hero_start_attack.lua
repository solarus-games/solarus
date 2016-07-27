local map = ...
local game = map:get_game()

function map:on_opening_transition_finished()

  game:set_ability("sword", 1)
  hero:start_attack()
  assert(hero:get_state() == "sword swinging")
  sol.main.exit()
end
