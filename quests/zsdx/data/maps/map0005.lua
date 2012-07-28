-- Outside world B4

function event_map_started(destination_point_name)

  -- enable dark world
  if sol.map.get_game():get_boolean(905) then
    sol.audio.play_music("dark_world")
    sol.map.tileset_set(13)
  else
    sol.audio.play_music("light_world")
  end

  -- don't allow to go to the surprise wall before dungeon 3 is finished
  if not sol.map.get_game():is_dungeon_finished(3) then
    sol.map.npc_remove("surprise_wall_guy")
  end

  if sol.map.get_game():get_boolean(136) then
    sol.map.tile_set_enabled("surprise_wall_door_tile", false)
    sol.map.npc_remove("surprise_wall_door")
  end
end

function event_npc_interaction(npc_name)

  if npc_name == "surprise_wall_guy" then
    if sol.map.tile_is_enabled("surprise_wall_door_tile") then
      sol.map.dialog_start("outside_world.surprise_wall_guy.closed")
    else
      sol.map.dialog_start("outside_world.surprise_wall_guy.open")
    end

  elseif npc_name == "surprise_wall_door" then
    sol.map.dialog_start("outside_world.surprise_wall_closed")
  end
end

function event_dialog_finished(dialog_id, answer)

  if dialog_id == "outside_world.surprise_wall_guy.closed" then
    if sol.map.get_game():get_item("level_4_way") == 1 then
      -- the player has the apple pie
      sol.map.dialog_start("outside_world.surprise_wall_guy.give_me_apple_pie")
    end
  elseif dialog_id == "outside_world.surprise_wall_guy.give_me_apple_pie"
      and answer == 0 then
    sol.map.get_game():set_item("level_4_way", 0)
    sol.map.dialog_start("outside_world.surprise_wall_guy.thanks")
  elseif dialog_id == "outside_world.surprise_wall_guy.thanks" then
    sol.map.tile_set_enabled("surprise_wall_door_tile", false)
    sol.map.npc_remove("surprise_wall_door")
    sol.map.get_game():set_boolean(136, true)
    sol.audio.play_sound("secret")
    sol.audio.play_sound("door_open")
  end
end

