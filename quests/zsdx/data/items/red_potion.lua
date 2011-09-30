function event_obtaining(variant, savegame_variable)

  local first_empty_bottle = get_first_empty_bottle()
  if first_empty_bottle ~= "" then
    sol.game.set_item(first_empty_bottle, 3)
  end
end

-- Returns the item name of an empty bottle, or an empty
-- string if there is no empty bottle
function get_first_empty_bottle()

  local result = ""

  if sol.game.get_item("bottle_1") == 1 then
    result = "bottle_1"
  elseif sol.game.get_item("bottle_2") == 1 then
    result = "bottle_2"
  elseif sol.game.get_item("bottle_3") == 1 then
    result = "bottle_3"
  elseif sol.game.get_item("bottle_4") == 1 then
    result = "bottle_4"
  end

  return result
end

