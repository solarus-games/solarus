---------------------------------
-- Dungeon 1 final room script --
---------------------------------

function event_map_started()
  freeze()
end

function event_opening_transition_finished()
  play_music("dungeon_finished.spc")
  npc_set_position("solarus_child", 160, 165)
  npc_set_animation("solarus_child", "stopped")
  start_timer(5000, "dialog", false)
end

function dialog()
  start_message("dungeon_1")
end

