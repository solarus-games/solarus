local map = ...
-- Outside world A1

function map:on_started(destination_point_name)

  -- game ending sequence
  if destination_point_name == "from_ending" then
    map:hero_freeze()
    map:hero_set_visible(false)
    map:get_game():set_hud_enabled(false)
    map:enemy_set_group_enabled("", false)
    sol.audio.play_music("fanfare")
  else
    -- enable dark world
    if map:get_game():get_boolean(905) then
      map:set_tileset(13)
    end
  end

  -- remove the dungeon 7 lock if open
  if map:get_game():get_boolean(919) then
    remove_dungeon_7_lock()
  end
end

function map:on_map_opening_transition_finished(destination_point_name)

  if destination_point_name == "from_ending" then
    map:start_dialog("credits_1")
    map:move_camera(1000, 240, 25, function() end, 1e6)
  end
end

function remove_dungeon_7_lock()
  map:npc_remove("dungeon_7_lock")
  map:tile_set_group_enabled("dungeon_7_lock_tile", false)
end

function map:on_npc_interaction(npc_name)

  if npc_name == "dungeon_7_lock" then

    -- open the door if the player has the ice key
    if map:get_game():has_item("ice_key") then
      sol.audio.play_sound("door_open")
      sol.audio.play_sound("secret")
      map:get_game():set_boolean(919, true)
      remove_dungeon_7_lock()
    else
      map:start_dialog("outside_world.ice_key_required")
    end
  end
end

function map:on_dialog_finished(dialog_id)

  if dialog_id == "credits_1" then
   sol.timer.start(2000, ending_next)
  end
end

function ending_next()
  map:hero_set_map(4, "from_ending", 1)
end

