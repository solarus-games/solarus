-- Dungeon 8 boss

function event_map_started(destination_point_name)

  sol.map.npc_set_enabled("ganon_npc", false)
  if destination_point_name == "from_1F" then
    if sol.map.get_game():get_boolean(728) then
      -- Agahnim already killed
      sol.map.npc_set_enabled("agahnim_npc", false)
    end
  end
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "start_boss_sensor" then

    if not sol.map.get_game():get_boolean(728) then

      sol.map.door_close("boss_door")
      sol.map.hero_freeze()
      sol.audio.play_music("agahnim")
      sol.main:start_timer(1000, function()
	sol.map.dialog_start("dungeon_8.agahnim")
      end)

    elseif not sol.game.is_dungeon_finished(8) then
      -- Agahnim already killed but Ganon's sequence not done yet
      -- (possible if the player dies or exits while Agahnim is dying)
      sol.map.hero_freeze()
      sol.main:start_timer(100, function()
        sol.map.hero_set_map(52, "ganon_dialog_destination_point", 1)
      end)
      sol.main:start_timer(200, start_ganon_sequence)
    end
  end
end

function event_dialog_finished(dialog_id)

  if dialog_id == "dungeon_8.agahnim" then
    sol.map.hero_set_map(52, "boss_destination_point", 1)
    sol.main:start_timer(100, function()
      sol.audio.play_music("ganon_battle")
      sol.map.enemy_set_enabled("boss", true)
      sol.map.npc_set_enabled("agahnim_npc", false)
    end)
  elseif dialog_id == "dungeon_8.ganon" then
    sol.audio.play_sound("world_warp")
    sol.main:start_timer(1000, function()
      sol.map.hud_set_pause_enabled(true)
      sol.map.hero_set_map(105, "from_outside", 1)
    end)
  end
end

function event_enemy_dead(enemy_name)

  if enemy_name == "boss" then
    sol.main:start_timer(1000, function()
      sol.audio.play_music("victory")
      sol.game.set_dungeon_finished(8)
      sol.map.hud_set_pause_enabled(false)
      sol.map.hero_freeze()
      sol.map.hero_set_direction(3)
      sol.main:start_timer(9000, function()
	sol.map.hero_set_map(52, "ganon_dialog_destination_point", 1)
      end)
      sol.main:start_timer(9100, start_ganon_sequence)
    end)
  end
end

function start_ganon_sequence()

  sol.map.hero_set_direction(1)
  sol.map.hero_freeze()
  sol.map.npc_set_enabled("ganon_npc", true)

  sol.main:start_timer(1000, function()
    sol.audio.play_music("ganon_theme")
    sol.map.dialog_start("dungeon_8.ganon")
  end)
end

