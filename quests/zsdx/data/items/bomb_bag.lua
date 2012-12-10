local item = ...

function item:on_created()

  self:set_savegame_variable("i1032")
end

function item:on_started()

  self:on_variant_changed(self:get_variant())
end

function item:on_variant_changed(variant)

  -- The bomb bag determines the maximum amount of the bomb counter.
  local bombs_counter = self:get_game():get_item("bombs_counter")
  local bomb = self:get_game():get_item("bomb")
  if variant == 0 then
    bombs_counter:set_max_amount(0)
    bomb:set_obtainable(false)
  else
    local max_amounts = {10, 30, 99}
    local max_amount = max_amounts[variant]

    -- Set the max value of the bomb counter.
    bombs_counter:set_variant(1)
    bombs_counter:set_max_amount(max_amount)

    -- Unlock pickable bombs.
    bomb:set_obtainable(true)
  end
end

function item:on_obtaining(variant, savegame_variable)

  if variant > 0 then
    local bombs_counter = self:get_game():get_item("bombs_counter")
    bombs_counter:set_amount(bombs_counter:get_max_amount())
  end
end

