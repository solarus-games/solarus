-- Dungeon 8 boss

function event_map_started(destination_point_name)

  sol.map.npc_set_enabled("ganon_npc", false)
  if destination_point_name == "from_1F" then
    if sol.game.savegame_get_boolean(728) then
      -- Agahnim already killed
      sol.map.npc_set_enabled("agahnim_npc", false)
    end
  end
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "start_boss_sensor" then

    if not sol.game.savegame_get_boolean(728) then

      sol.map.door_close("boss_door")
      sol.map.hero_freeze()
      sol.main.play_music("agahnim.spc")
      sol.main.timer_start(function()
	sol.map.dialog_start("dungeon_8.agahnim")
      end, 1000)

    elseif not sol.game.is_dungeon_finished(8) then
      -- Agahnim already killed but Ganon's sequence not done yet
      -- (possible if the player dies or exits while Agahnim is dying)
      sol.map.hero_freeze()
      sol.main.timer_start(function()
        sol.map.hero_set_map(52, "ganon_dialog_destination_point", 1)
      end, 100)
      sol.main.timer_start(start_ganon_sequence, 200)
    end
  end
end

function event_dialog_finished(first_message_id)

  if first_message_id == "dungeon_8.agahnim" then
    sol.map.hero_set_map(52, "boss_destination_point", 1)
    sol.main.timer_start(function()
      sol.main.play_music("ganon_battle.spc")
      sol.map.enemy_set_enabled("boss", true)
      sol.map.npc_set_enabled("agahnim_npc", false)
    end, 100)
  elseif first_message_id == "dungeon_8.ganon" then
    sol.main.play_sound("world_warp")
    sol.main.timer_start(function()
      sol.map.hud_set_pause_enabled(true)
      sol.map.hero_set_map(105, "from_outside", 1)
    end, 1000)
  end
end

function event_enemy_dead(enemy_name)

  if enemy_name == "boss" then
    sol.main.timer_start(function()
      sol.main.play_music("victory.spc")
      sol.game.set_dungeon_finished(8)
      sol.map.hud_set_pause_enabled(false)
      sol.map.hero_freeze()
      sol.map.hero_set_direction(3)
      sol.main.timer_start(function()
	sol.map.hero_set_map(52, "ganon_dialog_destination_point", 1)
      end, 9000)
      sol.main.timer_start(start_ganon_sequence, 9100)
    end, 1000)
  end
end

function start_ganon_sequence()

  sol.map.hero_set_direction(1)
  sol.map.hero_freeze()
  sol.map.npc_set_enabled("ganon_npc", true)

  sol.main.timer_start(function()
    sol.main.play_music("ganon_theme.spc")
    sol.map.dialog_start("dungeon_8.ganon")
  end, 1000)
end

