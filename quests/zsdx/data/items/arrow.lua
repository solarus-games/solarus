local item = ...

function item:on_started()

  self:set_shadow("small")
  self:set_can_disappear(true)
  self:set_brandish_when_picked(false)

  -- Disable pickable arrows if the player has no bow.
  local bow = self:get_game():get_item("bow")
  self:set_obtainable(bow:has_variant())
end

function item:on_obtaining(variant, savegame_variable)

  -- Obtaining arrows increases the counter of the bow.
  local amounts = {1, 5, 10}
  local amount = amounts[variant]
  if amount == nil then
    error("Invalid variant '" .. variant .. "' for item 'arrow'")
  end
  self:get_game():get_item("bow"):add_amount(amount)
end

