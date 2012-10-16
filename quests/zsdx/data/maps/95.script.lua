local map = ...
-- Potion shop

local function potion_buying(shop_item)

  local bottle_2 = map:get_game():get_item("bottle_2")
  if not map:get_game():get_boolean(911)
      and bottle_2:has_variant() then
    -- give bottle 2
    bottle_2:set_variant(1)
  end

  if map:get_first_empty_bottle() == nil then
    map:start_dialog("potion_shop.no_empty_bottle")
    return false
  end

  return true
end
red_potion.on_buying = potion_buying
green_potion.on_buying = potion_buying
blue_potion.on_buying = potion_buying

function hero:on_obtained_treasure(item_name, variant, savegame_variable)

  if item_name:find("_potion$")
      and not map:get_game():get_boolean(911) then
    -- tell the player we juste gave him the bottle 2
    map:get_game():set_boolean(911, true)
    map:start_dialog("potion_shop.give_bottle")
  end
end

function witch:on_interaction()

  if not map:get_game():get_boolean(911) then
    map:start_dialog("potion_shop.witch_bottle_offered")
  else
    map:start_dialog("potion_shop.witch")
  end
end

