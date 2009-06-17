-------------------------------------
-- Sahasrahla cave icy room script --
-------------------------------------

-- Function call when the map starts
function event_map_started()

   -- initialize the direction of the frozen door sprites
   interactive_entity_set_direction("frozen_door", 3)
   interactive_entity_set_direction("frozen_door_opposite", 1)
end

-- Function called when the player uses an inventory item on the frozen door
function event_interaction_item(entity_name, item_id, variant)

   if entity_name == "frozen_door" and
      inventory_item_is_bottle(item_id) and variant == 2 then

      -- using water on the frozen door
      play_sound("secret")
      return true
   else
      return false
   end
end
