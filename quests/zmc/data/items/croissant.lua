local item = ...

function item:on_obtaining(variant, savegame_variable)

  local croissants_counter = self:get_game():get_item("croissants_counter")
  if croissants_counter:get_variant() == 0 then
    croissants_counter:set_variant(1)
  end
  croissants_counter:add_amount(1)
end

