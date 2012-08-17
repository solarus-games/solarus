local map = ...
-- Dungeon 1 final room

function map:on_map_opening_transition_finished(destination_point_name)
  local solarus_child_sprite = map:npc_get_sprite("solarus_child")
  map:npc_set_position("solarus_child", 160, 165)
  solarus_child_sprite:set_animation("stopped")
  solarus_child_sprite:set_ignore_suspend(true)
end

function map:on_npc_interaction(npc_name)

  if npc_name == "solarus_child" then
    if map:get_game():is_dungeon_finished(1) then
      -- dialog already done
      sol.audio.play_sound("warp")
      map:hero_set_map(6, "from_dungeon_1_1F", 1)
    else
      -- start the final sequence
      map:camera_move(160, 120, 100, start_final_sequence)
    end
  end
end

function start_final_sequence()
  map:dialog_start("dungeon_1.solarus_child")
  map:dialog_set_variable("dungeon_1.solarus_child", map:get_game():get_player_name());
end

function map:on_dialog_finished(dialog_id, answer)

  if dialog_id == "dungeon_1.solarus_child" then
    map:hero_start_victory_sequence()
  end
end

function map:on_hero_victory_sequence_finished()
  map:get_game():set_dungeon_finished(1)
  map:hero_set_map(6, "from_dungeon_1_1F", 1)
  map:set_pause_enabled(true)
end

