---------------------------------
-- Dungeon 1 final room script --
---------------------------------

function event_map_started()
  play_music("dungeon_finished.spc")
  npc_set_position("solarus_child", 648, 117)
  npc_set_animation("solarus_child", "stopped")
end
