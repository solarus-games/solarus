-----------------------------
-- Outside world C2 script --
-----------------------------

-- Function called when the map starts
function event_map_started(destination_point_name)

   m = sol.main.random_path_movement_create(32)
   sol.map.npc_start_movement("chignon_woman", m)
   sol.main.sprite_set_animation(sol.map.npc_get_sprite("chignon_woman"), "walking")

   -- remove Tom's cave door if open
   if sol.game.savegame_get_boolean(36) then
      remove_village_cave_door()
   end

   -- broken rupee house
   if sol.game.savegame_get_boolean(155) then
     sol.map.teletransporter_set_enabled("to_rupee_house", false)
   else
     sol.map.teletransporter_set_enabled("to_broken_rupee_house", false)
     sol.map.tile_set_group_enabled("broken_rupee_house", false)
   end
end

-- Function called when the player presses the action key
-- while facing an interactive entity
function event_hero_interaction(entity_name)

   if entity_name == "tom_cave_door" then

      -- open the door if the player has the Clay Key
      if sol.game.has_item("clay_key") then
	 sol.main.play_sound("door_open")
	 sol.main.play_sound("secret")
	 sol.game.savegame_set_boolean(36, true)
	 remove_village_cave_door()
      else
	 sol.map.dialog_start("outside_world.village.clay_key_required")
      end
   end
end

function remove_village_cave_door()
   sol.map.interactive_entity_remove("tom_cave_door")
   sol.map.tile_set_enabled("tom_cave_door_tile", false)
end

