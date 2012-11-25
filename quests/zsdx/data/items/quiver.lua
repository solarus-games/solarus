local item = ...

function item:on_created()

  self:set_savegame_variable("i1033")
end

function item:on_variant_changed(variant)

  -- Obtaining a quiver changes the max amount of the bow.
  local max_amounts = {10, 30, 99}
  local max_amount = max_amounts[variant]
  if max_amount == nil then
    error("Invalid variant '" .. variant .. "' for item 'quiver'")
  end

  -- Set the max value of the bow counter.
  local bow = self:get_game():get_item("bow")
  bow:set_max_amount(max_amount)
  bow:set_amount(max_amount)

  -- Unlock pickable arrows.
  local arrow = self:get_game():get_item("arrow")
  arrow:set_obtainable(true)
end

