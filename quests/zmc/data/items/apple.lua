local item = ...

-- Obtaining 3 apples
function item:on_obtaining(variant, savegame_variable)

  self:get_game():get_item("apples_counter"):add_amount(3)
end

