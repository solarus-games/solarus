local item = ...

function item:on_created()

  self:set_can_disappear(true)
  self:set_brandish_when_picked(false)
end

function item:on_started()

  -- Disable pickable bombs if the player has no bomb bag.
  -- We cannot do this from on_created() because we don't know if the bomb bag
  -- is already created there.
  local bomb_bag = self:get_game():get_item("bomb_bag")
  self:set_obtainable(bomb_bag:has_variant())
end

function item:on_obtaining(variant, savegame_variable)

  -- Obtaining bombs increases the bombs counter.
  local amounts = {1, 3, 8}
  local amount = amounts[variant]
  if amount == nil then
    error("Invalid variant '" .. variant .. "' for item 'bomb'")
  end
  self:get_game():get_item("bombs_counter"):add_amount(amount)
end

