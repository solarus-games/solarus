local map = ...
-- Outside world B4

function map:on_started(destination_point_name)

  -- enable dark world
  if map:get_game():get_boolean(905) then
    sol.audio.play_music("dark_world")
    map:set_tileset(13)
  else
    sol.audio.play_music("overworld")
  end

  -- don't allow to go to the surprise wall before dungeon 3 is finished
  if not map:get_game():is_dungeon_finished(3) then
    map:npc_remove("surprise_wall_guy")
  end

  if map:get_game():get_boolean(136) then
    map:tile_set_enabled("surprise_wall_door_tile", false)
    map:npc_remove("surprise_wall_door")
  end
end

function map:on_npc_interaction(npc_name)

  if npc_name == "surprise_wall_guy" then
    if map:tile_is_enabled("surprise_wall_door_tile") then
      map:start_dialog("outside_world.surprise_wall_guy.closed")
    else
      map:start_dialog("outside_world.surprise_wall_guy.open")
    end

  elseif npc_name == "surprise_wall_door" then
    map:start_dialog("outside_world.surprise_wall_closed")
  end
end

function map:on_dialog_finished(dialog_id, answer)

  if dialog_id == "outside_world.surprise_wall_guy.closed" then
    if map:get_game():get_item("level_4_way") == 1 then
      -- the player has the apple pie
      map:start_dialog("outside_world.surprise_wall_guy.give_me_apple_pie")
    end
  elseif dialog_id == "outside_world.surprise_wall_guy.give_me_apple_pie"
      and answer == 0 then
    map:get_game():set_item("level_4_way", 0)
    map:start_dialog("outside_world.surprise_wall_guy.thanks")
  elseif dialog_id == "outside_world.surprise_wall_guy.thanks" then
    map:tile_set_enabled("surprise_wall_door_tile", false)
    map:npc_remove("surprise_wall_door")
    map:get_game():set_boolean(136, true)
    sol.audio.play_sound("secret")
    sol.audio.play_sound("door_open")
  end
end

