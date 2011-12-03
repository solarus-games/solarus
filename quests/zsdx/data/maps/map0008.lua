-- Outside world A1

function event_map_started(destination_point_name)

  -- game ending sequence
  if destination_point_name == "from_ending" then
    sol.map.hero_freeze()
    sol.map.hero_set_visible(false)
    sol.map.hud_set_enabled(false)
    sol.map.enemy_set_group_enabled("", false)
    sol.main.play_music("fanfare.spc")
  else
    -- enable dark world
    if sol.game.savegame_get_boolean(905) then
      sol.map.tileset_set(13)
    end
  end

  -- remove the dungeon 7 lock if open
  if sol.game.savegame_get_boolean(919) then
    remove_dungeon_7_lock()
  end
end

function event_map_opening_transition_finished(destination_point_name)

  if destination_point_name == "from_ending" then
    sol.map.dialog_start("credits_1")
    sol.map.camera_move(1000, 240, 50, function() end, 1e6)
  end
end

function remove_dungeon_7_lock()
  sol.map.npc_remove("dungeon_7_lock")
  sol.map.tile_set_group_enabled("dungeon_7_lock_tile", false)
end

function event_npc_interaction(npc_name)

  if npc_name == "dungeon_7_lock" then

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

function event_dialog_finished(first_message_id)

  if first_message_id == "credits_1" then
   sol.main.timer_start(ending_next, 2000)
  end
end

function ending_next()
  sol.map.hero_set_map(4, "from_ending", 1)
end

