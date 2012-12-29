local item = ...

function item:on_created()

  self:set_savegame_variable("i1033")
end

function item:on_started()

  self:on_variant_changed(self:get_variant())
end

function item:on_variant_changed(variant)

  -- The quiver determines the maximum amount of the bow.
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

    -- Unlock pickable arrows.
    arrow:set_obtainable(true)
  end
end

function item:on_obtaining(variant, savegame_variable)

  if variant > 0 then
    local bow = self:get_game():get_item("bow")
    bow:set_amount(bow:get_max_amount())
  end
end

