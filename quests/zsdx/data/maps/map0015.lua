---------------------------------------
-- Sahasrahla's cave icy room script --
---------------------------------------

-- Function call when the map starts
function event_map_started(destination_point_name)

   if savegame_get_boolean(35) then
      -- remove the frozen door
      interactive_entity_remove("frozen_door")
      interactive_entity_remove("frozen_door_opposite")
   else
      -- initialize the direction of the frozen door sprites
      interactive_entity_set_direction("frozen_door", 3)
      interactive_entity_set_direction("frozen_door_opposite", 1)
   end
end

-- Function called when the player presses the action key on the frozen door
function event_hero_interaction(entity_name)

   if entity_name == "frozen_door" then
      dialog_start("sahasrahla_house.frozen_door")
      savegame_set_boolean(34, true)
   end
end

-- Function called when the player uses an item on the frozen door
function event_hero_interaction_item(entity_name, item_name, variant)

   if entity_name == "frozen_door" and
      item_id.match("^bottle") and variant == 2 then

      -- using water on the frozen door
      hero_freeze()
      interactive_entity_set_animation("frozen_door", "disappearing")
      interactive_entity_set_animation("frozen_door_opposite", "disappearing")
      timer_start(800, "timer_frozen_door", false)
      return true
   end

   return false
end

-- Function called when the door is unfreezed
function timer_frozen_door()
   play_sound("secret")
   savegame_set_boolean(35, true)
   interactive_entity_remove("frozen_door")
   interactive_entity_remove("frozen_door_opposite")
   hero_unfreeze()
end
