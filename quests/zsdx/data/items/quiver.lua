local item = ...

function item:on_created()

  self:set_savegame_variable("i1033")
end

function item:on_started()

  self:on_variant_changed(self:get_variant())
end

function item:on_variant_changed(variant)

  -- Obtaining a quiver changes the max amount of the bow.
  local bow = self:get_game():get_item("bow")
  local arrow = self:get_game():get_item("arrow")
  if variant == 0 then
    bow:set_max_amount(0)
    arrow:set_obtainable(false)
  else
    local max_amounts = {10, 30, 99}
    local max_amount = max_amounts[variant]

    -- Set the max value of the bow counter.
    bow:set_max_amount(max_amount)
    bow:set_amount(max_amount)

    -- Unlock pickable arrows.
    arrow:set_obtainable(true)
  end
end

