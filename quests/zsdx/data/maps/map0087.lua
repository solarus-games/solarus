-- Dungeon 4 final room

function event_map_opening_transition_finished(destination_point_name)

  local solarus_child_sprite = sol.map.npc_get_sprite("solarus_child")
  sol.map.npc_set_position("solarus_child", 160, 165)
  solarus_child_sprite:set_animation("stopped")
  solarus_child_sprite:set_ignore_suspend(true)
end

function event_npc_interaction(npc_name)

  if npc_name == "solarus_child" then
    if sol.game.is_dungeon_finished(4) then
      -- dialog already done
      sol.audio.play_sound("warp")
      sol.map.hero_set_map(66, "from_dungeon_4", 1)
    else
      -- start the final sequence
      sol.map.camera_move(160, 120, 100, start_final_sequence, 0)
    end
  end
end

function start_final_sequence()
  sol.map.dialog_start("dungeon_4.solarus_child")
  sol.map.dialog_set_variable("dungeon_4.solarus_child", sol.map.get_game():get_player_name());
end

function event_dialog_finished(dialog_id, answer)

  if dialog_id == "dungeon_4.solarus_child" then
    sol.map.hero_start_victory_sequence()
  elseif dialog_id == "dungeon_4.agahnim" then
    agahnim_sequence_5()
  end
end

function event_hero_victory_sequence_finished()

  sol.main:start_timer(2000, agahnim_sequence_1)
end

function agahnim_sequence_1()

  sol.map.hero_unfreeze()
  sol.map.hero_set_direction(1)
  sol.main:start_timer(1000, agahnim_sequence_2)
end

function agahnim_sequence_2()

  local sprite = sol.map.npc_get_sprite("solarus_child")
  sprite:fade(1)
  sol.audio.stop_music()
  sol.audio.play_sound("warp")
  sol.main:start_timer(3000, agahnim_sequence_3)
end

function agahnim_sequence_3()

  sol.map.npc_set_position("agahnim", 160, 141)
  sol.audio.play_music("agahnim")
  sol.main:start_timer(1000, agahnim_sequence_4)
end

function agahnim_sequence_4()

  sol.map.dialog_start("dungeon_4.agahnim")
end

function agahnim_sequence_5()

-- FIXME not working
--  s = sol.map.npc_get_sprite("agahnim")
--  s:fade(1)
--  sol.audio.play_sound("warp")
  sol.main:start_timer(2000, agahnim_sequence_6)
end

function agahnim_sequence_6()

  sol.game.set_dungeon_finished(4)
  sol.map.get_game():set_boolean(905, true) -- enable the dark world
  sol.map.get_game():set_boolean(155, true) -- break the rupee house
  sol.map.hero_set_map(66, "from_dungeon_4", 1)
end

