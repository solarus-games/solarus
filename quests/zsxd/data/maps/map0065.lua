-- Temple of Stupidities 1F NE

will_remove_water = false

function event_map_started(destination_point_name)

  -- switches of stairs of the central room
  for i = 1,7 do
    if not sol.game.savegame_get_boolean(292 + i) then
      sol.map.stairs_set_enabled("stairs_"..i, false)
      sol.map.switch_set_activated("stairs_"..i.."_switch", false)
    else
      sol.map.switch_set_activated("stairs_"..i.."_switch", true)
    end
  end

  -- room with enemies to fight
  if sol.game.savegame_get_boolean(301) then
    sol.map.enemy_remove_group("fight_room")
  end

  -- block pushed to remove the water of 2F SW
  if sol.game.savegame_get_boolean(283) then
    sol.map.block_set_enabled("remove_water_block", false)
  else
    sol.map.block_set_enabled("fake_remove_water_block", false)
  end

  -- boss
  sol.map.door_set_open("boss_door", true)
  if sol.game.savegame_get_boolean(306) then
    sol.map.tile_set_enabled("boss_gate", true)
  end
end

function event_switch_activated(switch_name)

  i = string.match(switch_name, "^stairs_([1-7])_switch$")
  if (i ~= nil) then
    sol.map.stairs_set_enabled("stairs_"..i, true)
    sol.main.play_sound("secret")
    sol.game.savegame_set_boolean(292 + i, true)
  elseif switch_name == "switch_torch_1_on" then
    sol.map.tile_set_enabled("torch_1", true)
    sol.map.switch_set_activated("switch_torch_1_off", false)
  elseif switch_name == "switch_torch_1_off" then
    sol.map.tile_set_enabled("torch_1", false)
    sol.map.switch_set_activated("switch_torch_1_on", false)
  elseif switch_name == "switch_torch_2_on" then
    sol.map.tile_set_enabled("torch_2", true)
    sol.map.switch_set_activated("switch_torch_2_off", false)
  elseif switch_name == "switch_torch_2_off" then
    sol.map.tile_set_enabled("torch_2", false)
    sol.map.switch_set_activated("switch_torch_2_on", false)
  elseif switch_name == "switch_torch_3_on" then
    sol.map.tile_set_enabled("torch_3", true)
    sol.map.switch_set_activated("switch_torch_3_off", false)
  elseif switch_name == "switch_torch_3_off" then
    sol.map.tile_set_enabled("torch_3", false)
    sol.map.switch_set_activated("switch_torch_3_on", false)
  elseif switch_name == "switch_torch_4_on" then
    sol.map.tile_set_enabled("torch_4", true)
    sol.map.switch_set_activated("switch_torch_4_off", false)
  elseif switch_name == "switch_torch_4_off" then
    sol.map.tile_set_enabled("torch_4", false)
    sol.map.switch_set_activated("switch_torch_4_on", false)
  elseif switch_name == "switch_torch_5_on" then
    sol.map.tile_set_enabled("torch_5", true)
    sol.map.switch_set_activated("switch_torch_5_off", false)
  elseif switch_name == "switch_torch_5_off" then
    sol.map.tile_set_enabled("torch_5", false)
    sol.map.switch_set_activated("switch_torch_5_on", false)
  elseif switch_name == "switch_torch_6_on" then
    sol.map.tile_set_enabled("torch_6", true)
    sol.map.switch_set_activated("switch_torch_6_off", false)
  elseif switch_name == "switch_torch_6_off" then
    sol.map.tile_set_enabled("torch_6", false)
    sol.map.switch_set_activated("switch_torch_6_on", false)
  elseif switch_name == "switch_torch_7_on" then
    sol.map.tile_set_enabled("torch_7", true)
    sol.map.switch_set_activated("switch_torch_7_off", false)
    sol.map.switch_set_activated("switch_torch_7_off_2", false)
  elseif switch_name == "switch_torch_7_off"
      or switch_name == "switch_torch_7_off_2" then
    sol.map.tile_set_enabled("torch_7", false)
    sol.map.switch_set_activated("switch_torch_7_on", false)
  elseif switch_name == "switch_torch_8_on" then
    sol.map.tile_set_enabled("torch_8", true)
    sol.map.switch_set_activated("switch_torch_8_off", false)
  elseif switch_name == "switch_torch_8_off" then
    sol.map.tile_set_enabled("torch_8", false)
    sol.map.switch_set_activated("switch_torch_8_on", false)
  elseif switch_name == "switch_torch_9_on" then
    sol.map.tile_set_enabled("torch_9", true)
    sol.map.switch_set_activated("switch_torch_9_off", false)
  elseif switch_name == "switch_torch_9_off" then
    sol.map.tile_set_enabled("torch_9", false)
    sol.map.switch_set_activated("switch_torch_9_on", false)
  elseif switch_name == "switch_torch_10_on" then
    sol.map.tile_set_enabled("torch_10", true)
    sol.map.switch_set_activated("switch_torch_10_off", false)
  elseif switch_name == "switch_torch_10_off" then
    sol.map.tile_set_enabled("torch_10", false)
    sol.map.switch_set_activated("switch_torch_10_on", false)
  elseif switch_name == "switch_torch_11_on" then
    sol.map.tile_set_enabled("torch_11", true)
    sol.map.switch_set_activated("switch_torch_11_off", false)
  elseif switch_name == "switch_torch_11_off" then
    sol.map.tile_set_enabled("torch_11", false)
    sol.map.switch_set_activated("switch_torch_11_on", false)
  end
end

function event_enemy_dead(enemy_name)

  if string.match(enemy_name, '^fight_room')
      and sol.map.enemy_is_group_dead("fight_room") then

    sol.main.play_sound("secret")
    sol.map.door_open("fight_room_door")
  elseif enemy_name == "boss" then
    sol.map.tile_set_enabled("boss_gate", true) 
    sol.game.savegame_set_boolean(62, true) -- open the door of Link's cave
    sol.main.play_sound("secret")
  end
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "remove_water_sensor"
     and not sol.game.savegame_get_boolean(283)
     and not will_remove_water then

    sol.main.timer_start(500, "remove_2f_sw_water", false)
    will_remove_water = true
  elseif sensor_name == "start_boss_sensor" then
    if sol.map.door_is_open("boss_door") and not sol.game.savegame_get_boolean(306) then
      sol.map.door_close("boss_door")
      sol.map.hero_freeze()
      sol.main.timer_start(1000, "start_boss", false)
    end
  end
end

function remove_2f_sw_water()

  sol.main.play_sound("water_drain_begin")
  sol.main.play_sound("water_drain")
  sol.map.dialog_start("dungeon_1.2f_sw_water_removed")
  sol.game.savegame_set_boolean(283, true)
end

function start_boss()

  sol.map.enemy_set_enabled("boss", true)
  sol.main.play_music("ganon_theme.spc")
  sol.main.timer_start(1000, "ganon_dialog", false)
  sol.map.hero_unfreeze()
end

function ganon_dialog()
  sol.map.dialog_start("dungeon_1.ganon")
end

function event_hero_interaction(entity_name)

  if entity_name == "boss_hint_stone" then
    sol.main.timer_start(9000, "another_castle", false)
    sol.main.play_music("victory.spc")
    sol.map.hero_set_direction(3)
    sol.map.hero_freeze()
  end
end

function another_castle()

  sol.map.dialog_start("dungeon_1.boss_hint_stone")
  sol.map.dialog_set_variable("dungeon_1.boss_hint_stone", sol.game.savegame_get_name())
end

function event_dialog_finished(first_message_id)

  if first_message_id == "dungeon_1.boss_hint_stone" then
    sol.main.timer_start(1000, "victory", false)
  elseif first_message_id == "dungeon_1.ganon" then
    sol.main.play_music("ganon_battle.spc")
  end
end

function victory()
  sol.map.hero_start_victory_sequence()
  sol.main.timer_start(2000, "leave_dungeon", false)
end

function leave_dungeon()
  sol.main.play_sound("warp")
  sol.map.hero_set_map(4, "from_temple_of_stupidities", 1)
end

