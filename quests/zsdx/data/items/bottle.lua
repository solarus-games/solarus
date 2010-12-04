-- This script handles all bottles (each bottle script includes it)

-- TODO missing features in Lua API:
-- - create a pickable item on the map:
--   sol.map.create_pickable_item(item_name, x, y, layer) + falling height?
-- - detect the interaction between the item and an interactive entity
--   - when pressing the action key in front of the item:
--     for now, only the map script is notified (with event_hero_interaction)
--     solution 1: the CUSTOM subtype of InteractiveEntity can have a
--     paramater that tells which script should be notified
--     (by default, the map script)
--     solution 2 (much better): add a subtype of InteractiveEntity:
--     INTERACTION_WITH_ITEM with a parameter item_name (or just use CUSTOM!)
--   - when using the item explicitely: for now, only event_use() is called
--     and we ignore the existence of an interactive item
--     also notify the facing entity from C++ so that
--     event_interaction_with_item() is triggered
--   remove subtype WATER_FOR_BOTTLE from InteractiveEntity
--   parameters of InteractiveEntity:
--   - subtype: CUSTOM or NPC (NPCs are very specific, we keep them as interactive entities because they use the same parameters)
--   - name
--   - direction (0 to 4, -1 means any): the event is triggered only if the hero interacts with this direction of the entity
--   - sprite (if any)
--   - action_icon?
--   - behavior (when pressing the space key): "message#XXX" or "map" or "item#XXX" (no spaces), maybe more one day
--     - show a message
--     - call the map script (event_hero_interaction)
--     - call an item script (event_hero_interaction?)
--   (- info/additional_info/description/user_data: an arbitrary string to forward to the script notified
--     (may describe the entity better than just its name), for instance "water_for_bottle"
--   => need to replace message_id by behavior (more general) and to add user_data)

function event_use()

  variant = sol.item.get_variant()

  -- TODO switch
  -- empty bottle
  if variant == 1 then

    -- TODO interact with the facing entity if any
    -- sol.map.hero_interact...
    -- if !interaction_with_facing_entity then
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

    -- ask the hero to release the fairy
    sol.map.dialog_start("use_bottle_with_fairy")
  end
end

function event_dialog_finished(first_message_id, answer)

  if first_message_id == "use_bottle_with_water" then

    if answer == 0 then

      -- empty the water
      sol.item.set_variant(1) -- make the bottle empty
      sol.main.play_sound("item_in_water")

      -- TODO notify the facing entity (if any) of the interaction
      -- facing_entity->interaction_with_inventory_item(*this);

    end
    sol.item.set_finished()

  elseif first_message_id == "use_bottle_with_fairy" then

    if answer == 1 then

      -- release the fairy
      x, y, layer = sol.map.hero_get_position();
      sol.map.pickable_item_create("fairy", 1, -1, x, y, layer);
      sol.item.set_variant(1) -- make the bottle empty
    end
    sol.item.set_finished()
  end
end

