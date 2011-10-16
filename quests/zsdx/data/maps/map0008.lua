-- Outside world A1

function event_map_started(destination_point_name)

  -- enable dark world
  if sol.game.savegame_get_boolean(905) then
    sol.map.tileset_set(13)
  end

  -- remove the dungeon 7 lock if open
  if sol.game.savegame_get_boolean(919) then
    remove_dungeon_7_lock()
  end
end

function remove_dungeon_7_lock()
  sol.map.interactive_entity_remove("dungeon_7_lock")
  sol.map.tile_set_group_enabled("dungeon_7_lock_tile", false)
end

function event_hero_interaction(entity_name)

  if entity_name == "dungeon_7_lock" then

    -- open the door if the player has the ice key
    if sol.game.has_item("ice_key") then
      sol.main.play_sound("door_open")
      sol.main.play_sound("secret")
      sol.game.savegame_set_boolean(919, true)
      remove_dungeon_7_lock()
    else
      sol.map.dialog_start("outside_world.ice_key_required")
    end
  end
end

