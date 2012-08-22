local item = ...

function item:on_obtained(variant, savegame_variable)

  local game = self:get_game()
  game:add_max_life(4)
  game:set_life(game:get_max_life())
end

