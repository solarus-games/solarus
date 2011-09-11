-- Outside world B4

function event_map_started(destination_point_name)

  -- enable dark world
  if sol.game.savegame_get_boolean(905) then
    sol.main.play_music("dark_world.spc")
    sol.map.tileset_set(13)
  end

  -- don't allow to go to the surprise wall before dungeon 3 is finished
  if not sol.game.is_dungeon_finished(3) then
    sol.map.npc_remove("surprise_wall_guy")
  end

  if sol.game.savegame_get_boolean(136) then
    sol.map.tile_set_enabled("surprise_wall_door_tile", false)
    sol.map.interactive_entity_remove("surprise_wall_door")
  end
end

function event_npc_dialog(npc_name)

  if npc_name == "surprise_wall_guy" then
    if sol.map.tile_is_enabled("surprise_wall_door_tile") then
      sol.map.dialog_start("outside_world.surprise_wall_guy.closed")
    else
      sol.map.dialog_start("outside_world.surprise_wall_guy.open")
    end
  end
end

function event_dialog_finished(first_message_id, answer)

  if first_message_id == "outside_world.surprise_wall_guy.closed" then
    if sol.game.get_item("level_4_way") == 1 then
      -- the player has the apple pie
      sol.map.dialog_start("outside_world.surprise_wall_guy.give_me_apple_pie")
    end
  elseif first_message_id == "outside_world.surprise_wall_guy.give_me_apple_pie"
      and answer == 0 then
    sol.game.set_item("level_4_way", 0)
    sol.map.dialog_start("outside_world.surprise_wall_guy.thanks")
  elseif first_message_id == "outside_world.surprise_wall_guy.thanks" then
    sol.map.tile_set_enabled("surprise_wall_door_tile", false)
    sol.map.interactive_entity_remove("surprise_wall_door")
    sol.game.savegame_set_boolean(136, true)
    sol.main.play_sound("secret")
  end
end

function event_hero_interaction(entity_name)

  if entity_name == "surprise_wall_door" then
    sol.map.dialog_start("outside_world.surprise_wall_closed")
  end
end

