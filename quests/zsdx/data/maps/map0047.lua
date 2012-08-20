local map = ...
-- Dungeon 8 1F

-- Legend
-- RC: Rupee Chest
-- KC: Key Chest
-- KP: Key Pot
-- LD: Locked Door
-- KD: Key Door
-- DB: Door Button
-- LB: Locked Barrier
-- BB: Barrier Button
-- DS: Door Sensor

fighting_boss = false

function map:on_started(destination_point_name)
  map:door_set_open("LD1", true)
  map:door_set_open("LD3", true)
  map:door_set_open("LD4", true)
  map:npc_set_enabled("billy_npc", false)
  map:door_set_open("boss_door", true)

  -- Map chest hide if not already opened
  if not map:get_game():get_boolean(700) then
    map:chest_set_enabled("MAP", false)
  end

  -- Big key chest hide if not already opened
  if not map:get_game():get_boolean(705) then
    map:chest_set_enabled("BK01", false)
  end

  -- Link has mirror shield: no laser obstacles
  if map:get_game():get_ability("shield") >= 3 then
    map:wall_set_enabled("LO1", false)
    map:wall_set_group_enabled("LO2", false)
  end

  if destination_point_name == "from_boss" or destination_point_name == "from_hidden_room" then
    map:door_set_open("LD5", true)
  end

  if destination_point_name == "from_hidden_room" then
    map:enemy_remove_group("room_LD5_enemy")
  end

  -- door to Agahnim open if Billy's heart container was picked
  if map:get_game():get_boolean(729) then
    map:door_set_open("agahnim_door", true)
  end

  -- statues puzzle
  if map:get_game():get_boolean(723) then
    map:switch_set_activated("DB06", true)
  end

  -- boss key door and laser
  if map:get_game():get_boolean(730) then
    map:enemy_remove("boss_key_door_laser")
  end
end

function map:on_map_opening_transition_finished(destination_point_name)
  if destination_point_name == "from_outside" then
    map:start_dialog("dungeon_8.welcome")
  end
end

function map:on_switch_activated(switch_name)
  if switch_name == "BB1" then
    -- LB1 room
    map:move_camera(896, 1712, 250, BB1_remove_barrier)
  elseif switch_name == "BB2" then
    -- LB2 room
    map:tile_set_enabled("LB2", false)
    sol.audio.play_sound("secret")
  elseif switch_name == "DB4" then
    map:door_open("LD4")
    sol.audio.play_sound("secret")
  elseif switch_name == "DB06" then
    -- 4 statues room door opening
    map:door_open("LD6")
    sol.audio.play_sound("secret")
  elseif string.match(switch_name, "^RPS") then
    -- Resets position of statues
    map:block_reset("STT1")
    map:block_reset("STT2")
    map:block_reset("STT3")
    sol.audio.play_sound("warp")
  end
end

function BB1_remove_barrier()
  map:tile_set_enabled("LB1", false)
  sol.audio.play_sound("secret")
end

function map:on_hero_on_sensor(sensor_name)
  if sensor_name == "DS1" then
    -- LD1 room: when Link enters this room, door LD1 closes and enemies appear, sensor DS1 is disabled
    if not map:enemy_is_group_dead("room_LD1_enemy") then
      map:door_close("LD1")
      map:sensor_set_enabled("DS1", false)
    end
  elseif sensor_name == "DS3" then
    if not map:enemy_is_group_dead("map_enemy") then
      map:door_close("LD3")
      map:sensor_set_enabled("DS3", false)
    end
  elseif sensor_name == "DS4" then
    map:door_close("LD4")
    map:sensor_set_enabled("DS4", false)
  elseif sensor_name == "start_boss_sensor" then
    if not fighting_boss and not map:get_game():get_boolean(727) then
      sol.audio.stop_music()
      map:door_close("boss_door")
      map:npc_set_enabled("billy_npc", true)
      map:hero_freeze()
      fighting_boss = true
      sol.timer.start(1000, function()
	map:start_dialog("dungeon_8.billy")
      end)
    end
  end
end

function map:on_enemy_dead(enemy_name)
  if string.match(enemy_name, "^room_LD1_enemy") and map:enemy_is_group_dead("room_LD1_enemy") then
    -- LD1 room: kill all enemies will open the doors LD1 and LD2
    if not map:door_is_open("LD1") then
      map:door_open("LD1")
      map:door_open("LD2")
      sol.audio.play_sound("secret")
    end
  elseif string.match(enemy_name, "^room_LD5_enemy") and map:enemy_is_group_dead("room_LD5_enemy") and not map:door_is_open("LD5") then
    -- LD5 room: kill all enemies will open the door LD5
    map:door_open("LD5")
    sol.audio.play_sound("secret")
  elseif string.match(enemy_name, "^map_enemy") and map:enemy_is_group_dead("map_enemy") then
    -- Map chest room: kill all enemies and the chest will appear
    if not map:get_game():get_boolean(700) then
      map:chest_set_enabled("MAP", true)
      sol.audio.play_sound("chest_appears")
    elseif not map:door_is_open("LD3") then
      sol.audio.play_sound("secret")
    end
    map:door_open("LD3")
  elseif string.match(enemy_name, "^room_big_enemy") and map:enemy_is_group_dead("room_big_enemy") then
    -- Big key chest room: kill all enemies and the chest will appear
    if not map:get_game():get_boolean(705) then
      map:chest_set_enabled("BK01", true)
      sol.audio.play_sound("chest_appears")
    end
  end
end

function map:on_treasure_obtained(item_name, variant, savegame_variable)

  if item_name == "heart_container" then
    sol.audio.play_music("victory")
    map:hero_freeze()
    map:hero_set_direction(3)
    sol.timer.start(9000, function()
      map:door_open("boss_door")
      map:door_open("agahnim_door")
      sol.audio.play_sound("secret")
      map:hero_unfreeze()
    end)
  end
end

function map:on_dialog_finished(dialog_id)

  if dialog_id == "dungeon_8.billy" then
    sol.audio.play_music("boss")
    map:hero_unfreeze()
    map:enemy_set_enabled("boss", true)
    map:npc_set_enabled("billy_npc", false)
  end
end

function map:on_door_open(door_name)

  if door_name == "boss_key_door" then
    map:enemy_remove("boss_key_door_laser")
  elseif door_name == "WW01" then
    sol.audio.play_sound("secret")
  end
end

