local item = ...

function item:on_obtaining(variant, savegame_variable)

  local pains_au_chocolat_counter = self:get_game():get_item("pains_au_chocolat_counter")
  if pains_au_chocolat_counter:get_variant() == 0 then
    pains_au_chocolat_counter:set_variant(1)
  end
  pains_au_chocolat_counter:add_amount(1)
end

