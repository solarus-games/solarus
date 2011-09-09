-- Dungeon 4 final room

function event_map_opening_transition_finished(destination_point_name)
  solarus_child_sprite = sol.map.npc_get_sprite("solarus_child")
  sol.map.npc_set_position("solarus_child", 160, 165)
  sol.main.sprite_set_animation(solarus_child_sprite, "stopped")
  sol.main.sprite_set_animation_ignore_suspend(solarus_child_sprite, true)
end

function event_npc_dialog(npc_name)

  if npc_name == "solarus_child" then
    if sol.game.is_dungeon_finished(4) then
      -- dialog already done
      sol.main.play_sound("warp")
      sol.map.hero_set_map(66, "from_dungeon_4", 1)
    else
      -- start the final sequence
      sol.map.camera_move(160, 120, 100)
    end
  end
end

function event_camera_reached_target()
  sol.map.dialog_start("dungeon_4.solarus_child")
  sol.map.dialog_set_variable("dungeon_4.solarus_child", sol.game.savegame_get_name());
end

function event_dialog_finished(first_message_id, answer)

  if first_message_id == "dungeon_4.solarus_child" then
    sol.map.hero_start_victory_sequence()
  end
end

function event_hero_victory_sequence_finished()
  -- TODO
  --sol.game.set_dungeon_finished(4)
  --sol.map.hero_set_map(66, "from_dungeon_4", 1)
end


