local map = ...
-- Dungeon 8 boss

function map:on_started(destination_point_name)

  map:npc_set_enabled("ganon_npc", false)
  if destination_point_name == "from_1F" then
    if map:get_game():get_boolean(728) then
      -- Agahnim already killed
      map:npc_set_enabled("agahnim_npc", false)
    end
  end
end

function map:on_hero_on_sensor(sensor_name)

  if sensor_name == "start_boss_sensor" then

    if not map:get_game():get_boolean(728) then

      map:close_doors("boss_door")
      map:get_hero():freeze()
      sol.audio.play_music("agahnim")
      sol.timer.start(1000, function()
	map:start_dialog("dungeon_8.agahnim")
      end)

    elseif not map:get_game():is_dungeon_finished(8) then
      -- Agahnim already killed but Ganon's sequence not done yet
      -- (possible if the player dies or exits while Agahnim is dying)
      map:get_hero():freeze()
      sol.timer.start(100, function()
        map:get_hero():teleport(52, "ganon_dialog_destination_point")
      end)
      sol.timer.start(200, start_ganon_sequence)
    end
  end
end

function map:on_dialog_finished(dialog_id)

  if dialog_id == "dungeon_8.agahnim" then
    map:get_hero():teleport(52, "boss_destination_point")
    sol.timer.start(100, function()
      sol.audio.play_music("ganon_battle")
      map:enemy_set_enabled("boss", true)
      map:npc_set_enabled("agahnim_npc", false)
    end)
  elseif dialog_id == "dungeon_8.ganon" then
    sol.audio.play_sound("world_warp")
    sol.timer.start(1000, function()
      map:set_pause_enabled(true)
      map:get_hero():teleport(105, "from_outside")
    end)
  end
end

function map:on_enemy_dead(enemy_name)

  if enemy_name == "boss" then
    sol.timer.start(1000, function()
      sol.audio.play_music("victory")
      map:get_game():set_dungeon_finished(8)
      map:set_pause_enabled(false)
      map:get_hero():freeze()
      map:get_hero():set_direction(3)
      sol.timer.start(9000, function()
	map:get_hero():teleport(52, "ganon_dialog_destination_point")
      end)
      sol.timer.start(9100, start_ganon_sequence)
    end)
  end
end

function start_ganon_sequence()

  map:get_hero():set_direction(1)
  map:get_hero():freeze()
  map:npc_set_enabled("ganon_npc", true)

  sol.timer.start(1000, function()
    sol.audio.play_music("ganon_theme")
    map:start_dialog("dungeon_8.ganon")
  end)
end

