-- Dungeon 8 boss

function event_map_started(destination_point_name)

  if destination_point_name == "from_1F" then
    if sol.game.savegame_get_boolean(728)
        and not sol.game.savegame_get_boolean(729) then
      -- Agahnim killed, heart container not taken yet
      sol.map.hero_set_position(632, 485, 1)
    end
  end
  sol.map.npc_set_enabled("ganon_npc", false)
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "start_boss_sensor"
      and not sol.game.savegame_get_boolean(728) then

    sol.map.door_close("boss_door")
    sol.map.hero_freeze()
    sol.main.play_music("agahnim.spc")
    sol.main.timer_start(function()
      sol.map.dialog_start("dungeon_8.agahnim")
    end, 1000)
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

function event_treasure_obtained(item_name, variant, savegame_variable)

  if item_name == "heart_container" then
    sol.main.play_music("victory.spc")
    sol.game.set_dungeon_finished(8)
    sol.map.hud_set_pause_enabled(false)
    sol.map.hero_freeze()
    sol.map.hero_set_direction(3)
    sol.main.timer_start(function()
      sol.map.hero_set_map(52, "ganon_dialog_destination_point", 1)
    end, 9000)
    sol.main.timer_start(function()
      sol.map.hero_set_direction(1)
      sol.map.npc_set_enabled("ganon_npc", true)
    end, 9100)
    sol.main.timer_start(function()
      sol.main.play_music("ganon_theme.spc")
      sol.map.dialog_start("dungeon_8.ganon")
    end, 10000)
  end
end

