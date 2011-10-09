-- Dungeon 4 final room

function event_map_opening_transition_finished(destination_point_name)

  local solarus_child_sprite = sol.map.npc_get_sprite("solarus_child")
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
  elseif first_message_id == "dungeon_4.agahnim" then
    agahnim_sequence_5()
  end
end

function event_hero_victory_sequence_finished()

  sol.main.timer_start(agahnim_sequence_1, 2000)
end

function agahnim_sequence_1()

  sol.map.hero_unfreeze()
  sol.map.hero_set_direction(1)
  sol.main.timer_start(agahnim_sequence_2, 1000)
end

function agahnim_sequence_2()

  local sprite = sol.map.npc_get_sprite("solarus_child")
  sol.main.sprite_fade(sprite, 1)
  sol.main.play_music("none")
  sol.main.play_sound("warp")
  sol.main.timer_start(agahnim_sequence_3, 3000)
end

function agahnim_sequence_3()

  sol.map.npc_set_position("agahnim", 160, 141)
  sol.main.play_music("agahnim.spc")
  sol.main.timer_start(agahnim_sequence_4, 1000)
end

function agahnim_sequence_4()

  sol.map.dialog_start("dungeon_4.agahnim")
end

function agahnim_sequence_5()

-- FIXME not working
--  s = sol.map.npc_get_sprite("agahnim")
--  sol.main.sprite_fade(s, 1)
--  sol.main.play_sound("warp")
  sol.main.timer_start(agahnim_sequence_6, 2000)
end

function agahnim_sequence_6()

  sol.game.set_dungeon_finished(4)
  sol.game.savegame_set_boolean(905, true) -- enable the dark world
  sol.game.savegame_set_boolean(155, true) -- break the rupee house
  sol.map.hero_set_map(66, "from_dungeon_4", 1)
end

