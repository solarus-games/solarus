local item = ...

-- Called when the player uses the bombs of his inventory by pressing the corresponding item key
function item:on_use()

  if self:get_amount() == 0 then
    sol.audio.play_sound("wrong")
  else
    self:remove_amount(1)
    local x, y, layer = self:get_bomb_position_from_hero()
    self:get_map():bomb_create(x, y, layer)
    sol.audio.play_sound("bomb")
  end
  self:set_finished()
end

function item:get_bomb_position_from_hero()

  local map = self:get_map()
  local x, y, layer = map:hero_get_position()
  local direction = map:hero_get_direction()
  if direction == 0 then
    x = x + 16
  elseif direction == 1 then
    y = y - 16
  elseif direction == 2 then
    x = x - 16
  elseif direction == 3 then
    y = y + 16
  end

  return x, y, layer
end

