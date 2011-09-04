-- Outside world B2

function event_map_started(destination_point_name)

   -- remove the iron lock if open
   if sol.game.savegame_get_boolean(193) then
     remove_iron_lock()
   end

   -- remove the wooden lock if open
   if sol.game.savegame_get_boolean(194) then
     remove_wooden_lock()
   end

end

function remove_iron_lock()
   sol.map.interactive_entity_remove("iron_lock")
   sol.map.tile_set_group_enabled("iron_lock_tile", false)
end

function remove_wooden_lock()
   sol.map.interactive_entity_remove("wooden_lock")
   sol.map.tile_set_group_enabled("wooden_lock_tile", false)
end


function event_hero_interaction(entity_name)

  if entity_name == "iron_lock" then

    -- open the door if the player has the iron key
    if sol.game.has_item("iron_key") then
      sol.main.play_sound("door_open")
      sol.main.play_sound("secret")
      sol.game.savegame_set_boolean(193, true)
      remove_iron_lock()
    else
      sol.map.dialog_start("outside_world.iron_key_required")
    end
  elseif entity_name == "wooden_lock" then

    -- open the door if the player has the wooden key
    if sol.game.has_item("wooden_key") then
      sol.main.play_sound("door_open")
      sol.main.play_sound("secret")
      sol.game.savegame_set_boolean(194, true)
      remove_wooden_lock()
    else
      sol.map.dialog_start("outside_world.wooden_key_required")
    end
  end
end

