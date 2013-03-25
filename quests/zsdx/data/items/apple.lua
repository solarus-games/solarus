local item = ...

-- Obtaining 3 apples
function item:on_obtaining(variant, savegame_variable)

  local apples_counter = self:get_game():get_item("apples_counter")
  if apples_counter:get_variant() == 0 then
    apples_counter:set_variant(1)
  end
  apples_counter:add_amount(3)
end

