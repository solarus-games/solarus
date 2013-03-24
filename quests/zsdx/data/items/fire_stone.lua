local item = ...

function item:on_obtaining(variant, savegame_variable)

  local fire_stones_counter = self:get_game():get_item("fire_stones_counter")
  if fire_stones_counter:get_variant() == 0 then
    fire_stones_counter:set_variant(1)
  end
  fire_stones_counter:add_amount(1)
end

