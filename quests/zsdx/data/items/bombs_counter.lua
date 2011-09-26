-- Called when the player uses the bombs of his inventory by pressing the corresponding item key
function event_use()

  if sol.item.get_amount() == 0 then
    sol.main.play_sound("wrong")
  else
    sol.item.remove_amount(1)
    local x, y, layer = get_bomb_position_from_hero()
    sol.map.bomb_create(x, y, layer)
    sol.main.play_sound("bomb")
  end
  sol.item.set_finished()
end

function get_bomb_position_from_hero()

  local x, y, layer = sol.map.hero_get_position()
  local direction = sol.map.hero_get_direction()
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
