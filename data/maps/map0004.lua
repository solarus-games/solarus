-----------------------------
-- Outside world C2 script --
-----------------------------

-- Function called when the map starts
function event_map_started(destination_point_name)

   npc_random_walk("chignon_woman")

   -- remove the door if open
   if savegame_get_boolean(36) then
      remove_village_cave_door()
   end
end

function event_hero_on_sensor(sensor_name)

  if string.find(sensor_name, "not_in_demo_sensor") then
    start_message("outside_world.not_in_demo")
  end
end

-- Function called when the player presses the action key
-- while facing an interactive entity
function event_interaction(entity_name)

   if entity_name == "village_cave_door" then

      -- open the door if the player has the Clay Key
      if inventory_item_get(16) ~= 0 then
	 play_sound("door_open")
	 play_sound("secret")
	 savegame_set_boolean(36, true)
	 remove_village_cave_door()
      else
	 start_message("outside_world.village.clay_key_required")
      end
   end
end

function remove_village_cave_door()
   interactive_entity_remove("village_cave_door")
   tile_set_enabled("village_cave_hiding", false)
   tile_set_enabled("village_cave_hiding_2", false)
end
