---------------------------------------
-- Sahasrahla's cave icy room script --
---------------------------------------

frozen_door_sprite = nil
frozen_door_opposite_sprite = nil

-- Function call when the map starts
function event_map_started(destination_point_name)

   if sol.game.savegame_get_boolean(35) then
      -- remove the frozen door
      sol.map.interactive_entity_remove("frozen_door")
      sol.map.interactive_entity_remove("frozen_door_opposite")
   else
      -- initialize the direction of the frozen door sprites
      frozen_door_sprite = sol.map.interactive_entity_get_sprite("frozen_door")
      frozen_door_opposite_sprite = sol.map.interactive_entity_get_sprite("frozen_door_opposite")
      sol.main.sprite_set_direction(frozen_door_sprite, 3)
      sol.main.sprite_set_direction(frozen_door_opposite_sprite, 1)
   end
end

-- Function called when the player presses the action key on the frozen door
function event_hero_interaction(entity_name)

   if entity_name == "frozen_door" then
      sol.map.dialog_start("sahasrahla_house.frozen_door")
      sol.game.savegame_set_boolean(34, true)
   end
end

-- Function called when the player uses an item on the frozen door
function event_hero_interaction_item(entity_name, item_name, variant)

   if entity_name == "frozen_door" and
      string.match(item_name, "^bottle") and variant == 2 then

      -- using water on the frozen door
      sol.map.hero_freeze()
      sol.main.play_sound("item_in_water")
      sol.main.sprite_set_animation(frozen_door_sprite, "disappearing")
      sol.main.sprite_set_animation(frozen_door_opposite_sprite, "disappearing")
      sol.main.timer_start(800, "timer_frozen_door", false)
      sol.game.set_item(item_name, 1) -- make the bottle empty
      return true
   end

   return false
end

-- Function called when the door is unfreezed
function timer_frozen_door()
   sol.main.play_sound("secret")
   sol.game.savegame_set_boolean(35, true)
   sol.map.interactive_entity_remove("frozen_door")
   sol.map.interactive_entity_remove("frozen_door_opposite")
   sol.map.hero_unfreeze()
end

