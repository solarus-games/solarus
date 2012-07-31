local item = ...

function item:obtaining(variant, savegame_variable)

  local first_empty_bottle = self:get_first_empty_bottle()
  if first_empty_bottle ~= "" then
    self:get_game():set_item(first_empty_bottle, 5)
  end
end

-- Returns the item name of an empty bottle, or an empty
-- string if there is no empty bottle
function item:get_first_empty_bottle()

  local result = ""

  if self:get_game():get_item("bottle_1") == 1 then
    result = "bottle_1"
  elseif self:get_game():get_item("bottle_2") == 1 then
    result = "bottle_2"
  elseif self:get_game():get_item("bottle_3") == 1 then
    result = "bottle_3"
  elseif self:get_game():get_item("bottle_4") == 1 then
    result = "bottle_4"
  end

  return result
end



