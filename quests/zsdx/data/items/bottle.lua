-- This script handles all bottles (each bottle script includes it)

bottle_to_make_empty = "" -- name of a bottle to make empty at the next cycle

function event_use()

  variant = sol.item.get_variant()

  -- TODO switch
  -- empty bottle
  if variant == 1 then
    sol.main.play_sound("wrong")
    sol.item.set_finished()

    -- water
  elseif variant == 2 then

    -- ask the hero to pour away the water
    sol.map.dialog_start("use_bottle_with_water")

    -- red potion
  elseif variant == 3 then
    -- TODO
    sol.item.set_finished()

    -- green potion
  elseif variant == 4 then
    -- TODO
    sol.item.set_finished()

    -- blue potion
  elseif variant == 5 then
    -- TODO
    sol.item.set_finished()

    -- fairy
  elseif variant == 6 then

      -- release the fairy
      x, y, layer = sol.map.hero_get_position();
      sol.map.pickable_item_create("fairy", 1, -1, x, y, layer);
      sol.item.set_variant(1) -- make the bottle empty
      sol.item.set_finished()
  end
end

function event_dialog_finished(first_message_id, answer)

  if first_message_id == "use_bottle_with_water" then

    if answer == 0 then

      -- empty the water
      sol.item.set_variant(1) -- make the bottle empty
      sol.main.play_sound("item_in_water")

    end
    sol.item.set_finished()

  elseif first_message_id == "found_water" then

    if answer == 0 then
      sol.map.treasure_give(get_first_empty_bottle(), 2, -1)
    end
  
  end
end

function event_hero_interaction(entity_name)

  if string.match(entity_name, "^water_for_bottle") then
    -- the hero interacts with a place where he can get some water
    if has_bottle() then
      if has_empty_bottle() then
        sol.map.dialog_start("found_water")
      else
        sol.map.dialog_start("found_water.no_empty_bottle")
      end
    else  
      sol.map.dialog_start("found_water.no_bottle")
    end
  end
end

function event_hero_interaction_item(entity_name, item_name, variant)

  if string.match(item_name, "^bottle") and string.match(entity_name, "^water_for_bottle") then
    -- the hero interacts with a place where he can get some water:
    -- no matter whether he pressed the action key or the item key of a bottle, we do the same thing
    event_hero_interaction(entity_name)
    return true
  end

  return false
end

function event_variant_changed(variant)

  -- the possession state of a bottle has changed: see if the player has at least a fairy
  if has_bottle_with(6) then
    sol.game.set_ability("get_back_from_death", 1)
  else
    sol.game.set_ability("get_back_from_death", 0)
  end
end

function event_ability_used(ability_name)

  -- if the hero was just saved by a fairy,
  -- let's find a bottle with a fairy and make it empty

  -- remember that all bottles are notified, but only
  -- one should remove its fairy

  if ability_name == "get_back_from_death"
    and sol.game.has_ability("get_back_from_death") then

    bottle_to_make_empty = get_first_bottle_with(6)
  end
end

function event_update()

  if bottle_to_make_empty ~= "" then
    sol.game.set_item(bottle_to_make_empty, 1)
  end
end

function has_bottle()

  return sol.game.has_item("bottle_1")
    or sol.game.has_item("bottle_2")
    or sol.game.has_item("bottle_3")
    or sol.game.has_item("bottle_4")
end

function has_empty_bottle()

  return get_first_empty_bottle() ~= ""
end

function has_bottle_with(variant)

  return get_first_bottle_with(variant) ~= ""
end

function get_first_empty_bottle()

  return get_first_bottle_with(1)
end

function get_first_bottle_with(variant)

  result = ""

  if sol.game.get_item("bottle_1") == variant then
    result = "bottle_1"
  elseif sol.game.get_item("bottle_2") == variant then
    result = "bottle_2"
  elseif sol.game.get_item("bottle_3") == variant then
    result = "bottle_3"
  elseif sol.game.get_item("bottle_4") == variant then
    result = "bottle_4"
  end

  return result
end