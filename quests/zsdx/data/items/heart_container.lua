local item = ...

function item:on_obtained(variant, savegame_variable)

  self:get_game():add_max_life(4)
  self:get_game():add_life(self:get_game():get_max_life())
end

