local map = ...
-- Potion shop

function map:on_shop_item_buying(item_name)

  if not map:get_game():get_boolean(911)
      and not map:get_game():has_item("bottle_2") then
    -- give bottle 2
    map:get_game():set_item("bottle_2", 1)
  end

  if get_first_empty_bottle() == "" then
    map:start_dialog("potion_shop.no_empty_bottle")
    return false
  end

  return true
end

function map:on_treasure_obtained(item_name, variant, savegame_variable)

  if string.find(item_name, "_potion$")
      and not map:get_game():get_boolean(911) then
    -- tell the player we juste gave him the bottle 2
    map:get_game():set_boolean(911, true)
    map:start_dialog("potion_shop.give_bottle")
  end
end

function map:on_npc_interaction(npc_name)

  if npc_name == "witch" then

    if not map:get_game():get_boolean(911) then
      map:start_dialog("potion_shop.witch_bottle_offered")
    else
      map:start_dialog("potion_shop.witch")
    end
  end
end

-- Returns the item name of an empty bottle, or an empty
-- string if there is no empty bottle
function get_first_empty_bottle()

  local result = ""

  if map:get_game():get_item("bottle_1") == 1 then
    result = "bottle_1"
  elseif map:get_game():get_item("bottle_2") == 1 then
    result = "bottle_2"
  elseif map:get_game():get_item("bottle_3") == 1 then
    result = "bottle_3"
  elseif map:get_game():get_item("bottle_4") == 1 then
    result = "bottle_4"
  end

  return result
end

