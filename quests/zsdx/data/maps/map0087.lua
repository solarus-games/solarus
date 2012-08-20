local map = ...
-- Dungeon 4 final room

function map:on_map_opening_transition_finished(destination_point_name)

  local solarus_child_sprite = map:npc_get_sprite("solarus_child")
  map:npc_set_position("solarus_child", 160, 165)
  solarus_child_sprite:set_animation("stopped")
  solarus_child_sprite:set_ignore_suspend(true)
end

function map:on_npc_interaction(npc_name)

  if npc_name == "solarus_child" then
    if map:get_game():is_dungeon_finished(4) then
      -- dialog already done
      sol.audio.play_sound("warp")
      map:hero_set_map(66, "from_dungeon_4", 1)
    else
      -- start the final sequence
      map:move_camera(160, 120, 100, start_final_sequence, 0)
    end
  end
end

function start_final_sequence()
  map:start_dialog("dungeon_4.solarus_child")
  map:set_dialog_variable("dungeon_4.solarus_child", map:get_game():get_player_name());
end

function map:on_dialog_finished(dialog_id, answer)

  if dialog_id == "dungeon_4.solarus_child" then
    map:hero_start_victory_sequence()
  elseif dialog_id == "dungeon_4.agahnim" then
    agahnim_sequence_5()
  end
end

function map:on_hero_victory_sequence_finished()

  sol.timer.start(2000, agahnim_sequence_1)
end

function agahnim_sequence_1()

  map:hero_unfreeze()
  map:hero_set_direction(1)
  sol.timer.start(1000, agahnim_sequence_2)
end

function agahnim_sequence_2()

  local sprite = map:npc_get_sprite("solarus_child")
  sprite:fade_out()
  sol.audio.stop_music()
  sol.audio.play_sound("warp")
  sol.timer.start(3000, agahnim_sequence_3)
end

function agahnim_sequence_3()

  map:npc_set_position("agahnim", 160, 141)
  sol.audio.play_music("agahnim")
  sol.timer.start(1000, agahnim_sequence_4)
end

function agahnim_sequence_4()

  map:start_dialog("dungeon_4.agahnim")
end

function agahnim_sequence_5()

-- FIXME not working
--  s = map:npc_get_sprite("agahnim")
--  s:fade_in()
--  sol.audio.play_sound("warp")
  sol.timer.start(2000, agahnim_sequence_6)
end

function agahnim_sequence_6()

  map:get_game():set_dungeon_finished(4)
  map:get_game():set_boolean(905, true) -- enable the dark world
  map:get_game():set_boolean(155, true) -- break the rupee house
  map:hero_set_map(66, "from_dungeon_4", 1)
end

