local item = ...

function item:on_created()

  self:set_shadow("small")
  self:set_can_disappear(true)
  self:set_brandish_when_picked(false)
end

function item:on_started()

  -- Disable pickable arrows if the player has no bow.
  -- We cannot do this from on_created() because we don't know if the bow
  -- is already created there.
  self:set_obtainable(self:get_game():has_item("bow"))
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

