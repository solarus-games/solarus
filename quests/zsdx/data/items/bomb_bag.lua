local item = ...

function item:on_created()

  self:set_savegame_variable("i1032")
end

function item:on_variant_changed(variant)

  if variant > 0 then
    -- Obtaining a bomb bag changes the max amount of the bomb counter.
    local max_amounts = {10, 30, 99}
    local max_amount = max_amounts[variant]
    if max_amount == nil then
      error("Invalid variant '" .. variant .. "' for item 'bomb_bag'")
    end

    -- Set the max value of the bomb counter.
    local bombs_counter = self:get_game():get_item("bombs_counter")
    bombs_counter:set_variant(1)
    bombs_counter:set_max_amount(max_amount)
    bombs_counter:set_amount(max_amount)

    -- Unlock pickable bombs.
    local bomb = self:get_game():get_item("bomb")
    bomb:set_obtainable(true)
  end
end

