local item = ...

function item:on_map_changed(map)

  local game = self:get_game()
  local savegame_variable = game:get_small_keys_savegame_variable()

  if savegame_variable == nil then
    item:set_amount(0)  -- No small key counter on this map
  else
    item:set_amount(game:get_value(savegame_variable) or 0)
  end
end

function item:on_amount_changed(amount)

  local game = self:get_game()
  local savegame_variable = game:get_small_keys_savegame_variable()

  if savegame_variable ~= nil then
    game:set_value(savegame_variable, amount)
  end
end

