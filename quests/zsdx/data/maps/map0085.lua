local map = ...
-- Dungeon 3 final room

function map:on_opening_transition_finished(destination_point)
  local solarus_child_sprite = solarus_child:get_sprite()
  map:npc_set_position("solarus_child", 160, 165)
  solarus_child_sprite:set_animation("stopped")
  solarus_child_sprite:set_ignore_suspend(true)
end

function map:on_npc_interaction(npc_name)

  if npc_name == "solarus_child" then
    if map:get_game():is_dungeon_finished(3) then
      -- dialog already done
      sol.audio.play_sound("warp")
      map:get_hero():teleport(3, "from_dungeon_3")
    else
      -- start the final sequence
      map:move_camera(160, 120, 100, start_final_sequence, 0)
    end
  end
end

function start_final_sequence()
  map:start_dialog("dungeon_3.solarus_child")
  map:set_dialog_variable("dungeon_3.solarus_child", map:get_game():get_player_name())
end

function map:on_dialog_finished(dialog_id, answer)

  if dialog_id == "dungeon_3.solarus_child" then
    map:get_hero():start_victory()
  end

end

function map:on_hero_victory_finished()
  map:get_game():set_dungeon_finished(3)
  map:get_hero():teleport(3, "from_dungeon_3")
end

