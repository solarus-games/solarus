-- Potion shop

function event_shop_item_buying(item_name)

  if not sol.game.savegame_get_boolean(911)
      and not sol.game.has_item("bottle_2") then
    -- give bottle 2
    sol.game.set_item("bottle_2", 1)
  end

  if get_first_empty_bottle() == "" then
    sol.map.dialog_start("potion_shop.no_empty_bottle")
    return false
  end

  return true
end

function event_treasure_obtained(item_name, variant, savegame_variable)

  if string.find(item_name, "_potion$")
      and not sol.game.savegame_get_boolean(911) then
    -- tell the player we juste gave him the bottle 2
    sol.game.savegame_set_boolean(911, true)
    sol.map.dialog_start("potion_shop.give_bottle")
  end
end

function event_npc_dialog(npc_name)

  if npc_name == "witch" then

    if not sol.game.savegame_get_boolean(911) then
      sol.map.dialog_start("potion_shop.witch_bottle_offered")
    else
      sol.map.dialog_start("potion_shop.witch")
    end
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

