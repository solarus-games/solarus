---------------------------------
-- Dungeon 1 final room script --
---------------------------------

function event_map_started()
  freeze()
end

function event_opening_transition_finished(destination_point_name)
  play_music("dungeon_finished.spc")
  npc_set_position("solarus_child", 160, 165)
  npc_set_animation("solarus_child", "stopped")
  npc_set_animation_ignore_suspend("solarus_child", true)
  start_timer(5000, "dialog", false)
end

function dialog()
  start_message("dungeon_1.solarus_child")
  set_message_variable("dungeon_1.solarus_child", player_get_name());
end

function event_message_sequence_finished(first_message_id, answer)

  if first_message_id == "dungeon_1.solarus_child" then
    hero_start_victory_sequence()
  end

end

function event_hero_victory_sequence_finished()
  hero_set_map(6, "from_dungeon_1_1F", 1)
  player_set_pause_enabled(true);
end

