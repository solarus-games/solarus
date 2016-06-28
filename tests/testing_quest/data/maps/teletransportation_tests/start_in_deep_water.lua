local map = ...
local game = map:get_game()

function map:on_started()

  game:set_ability("swim", 1)
end
