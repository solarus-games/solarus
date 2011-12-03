-- Dungeon 9 boss

function event_map_started(destination_point_name)

  if not sol.game.savegame_get_boolean(881) then
    sol.main.play_music("ganon_appears.spc")
    sol.map.enemy_set_enabled("boss", true)
    sol.map.npc_set_enabled("zelda", false)
    sol.map.npc_set_group_enabled("child", false)
  end
end

function event_map_opening_transition_finished(destination_point_name)

  if destination_point_name == "from_6f" then
    if not sol.game.savegame_get_boolean(881) then
      sol.map.dialog_start("dungeon_9.boss")
    else
      start_zelda_sequence()
    end
  end
end

function event_dialog_finished(first_message_id)

  if first_message_id == "dungeon_9.boss" then
    sol.main.play_music("ganon_battle.spc")
  elseif first_message_id == "dungeon_9.zelda" then
    sol.main.timer_start(function()
      sol.map.dialog_start("dungeon_9.zelda_children")
    end, 1000)
  elseif first_message_id == "dungeon_9.zelda_children" then
    sol.main.play_music("none")
    sol.main.play_sound("world_warp")
    sol.main.timer_start(function()
      for i = 1, 8 do
	sol.main.sprite_fade(sol.map.npc_get_sprite("child_" .. i), 1)
      end
    end, 1000)
    sol.main.timer_start(function()
      sol.map.dialog_start("dungeon_9.zelda_end")
    end, 5000)
  elseif first_message_id == "dungeon_9.zelda_end" then
    sol.main.timer_start(function()
      sol.map.hero_set_map(8, "from_ending", 1)
    end, 2000)
  end
end

function event_enemy_dead(enemy_name)

  if enemy_name == "boss" then
    sol.main.timer_start(start_final_sequence, 1000)
  end
end

function start_final_sequence()

  sol.map.hero_freeze()
  sol.map.hero_set_direction(3)
  sol.main.play_music("victory.spc")
  sol.main.timer_start(function()
    sol.map.hero_set_map(130, "from_boss", 1)
  end, 9000)
  sol.main.timer_start(start_zelda_sequence, 9100)
end

function start_zelda_sequence()

  sol.main.play_music("triforce.spc")
  sol.map.hero_freeze()
  sol.map.hero_set_direction(1)
  sol.map.npc_set_enabled("zelda", true)
  for i = 1, 8 do
    local npc_name = "child_" .. i
    sol.map.npc_set_enabled(npc_name, true)
    local sprite = sol.map.npc_get_sprite(npc_name)
    sol.main.sprite_set_animation_ignore_suspend(sprite, true)
    sol.main.sprite_fade(sprite, 0)
  end

  sol.main.timer_start(function()
    sol.map.dialog_start("dungeon_9.zelda")
    sol.map.dialog_set_variable("dungeon_9.zelda", sol.game.savegame_get_name())
  end, 3000)
end

