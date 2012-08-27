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

function map:on_started(destination_point)
  map:set_doors_open("LD1", true)
  map:set_doors_open("LD3", true)
  map:set_doors_open("LD4", true)
  billy_npc:set_enabled(false)
  map:set_doors_open("boss_door", true)

  -- Map chest hide if not already opened
  if not map:get_game():get_boolean(700) then
    MAP:set_enabled(false)
  end

  -- Big key chest hide if not already opened
  if not map:get_game():get_boolean(705) then
    BK01:set_enabled(false)
  end

  -- Link has mirror shield: no laser obstacles
  if map:get_game():get_ability("shield") >= 3 then
    LO1:set_enabled(false)
    map:set_entities_enabled("LO2", false)
  end

  if destination_point:get_name() == "from_boss" or destination_point == "from_hidden_room" then
    map:set_doors_open("LD5", true)
  end

  if destination_point:get_name() == "from_hidden_room" then
    map:remove_entities("room_LD5_enemy")
  end

  -- door to Agahnim open if Billy's heart container was picked
  if map:get_game():get_boolean(729) then
    map:set_doors_open("agahnim_door", true)
  end

  -- statues puzzle
  if map:get_game():get_boolean(723) then
    DB06:set_activated(true)
  end

  -- boss key door and laser
  if map:get_game():get_boolean(730) then
    boss_key_door_laser:remove()
  end
end

function map:on_opening_transition_finished(destination_point)
  if destination_point:get_name() == "from_outside" then
    map:start_dialog("dungeon_8.welcome")
  end
end

function map:on_switch_activated(switch_name)
  if switch_name == "BB1" then
    -- LB1 room
    map:move_camera(896, 1712, 250, BB1_remove_barrier)
  elseif switch_name == "BB2" then
    -- LB2 room
    LB2:set_enabled(false)
    sol.audio.play_sound("secret")
  elseif switch_name == "DB4" then
    map:open_doors("LD4")
    sol.audio.play_sound("secret")
  elseif switch_name == "DB06" then
    -- 4 statues room door opening
    map:open_doors("LD6")
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
  LB1:set_enabled(false)
  sol.audio.play_sound("secret")
end

function map:on_hero_on_sensor(sensor_name)
  if sensor_name == "DS1" then
    -- LD1 room: when Link enters this room, door LD1 closes and enemies appear, sensor DS1 is disabled
    if map:has_entities("room_LD1_enemy") then
      map:close_doors("LD1")
      DS1:set_enabled(false)
    end
  elseif sensor_name == "DS3" then
    if map:has_entities("map_enemy") then
      map:close_doors("LD3")
      DS3:set_enabled(false)
    end
  elseif sensor_name == "DS4" then
    map:close_doors("LD4")
    DS4:set_enabled(false)
  elseif sensor_name == "start_boss_sensor" then
    if not fighting_boss and not map:get_game():get_boolean(727) then
      sol.audio.stop_music()
      map:close_doors("boss_door")
      billy_npc:set_enabled(true)
      hero:freeze()
      fighting_boss = true
      sol.timer.start(1000, function()
	map:start_dialog("dungeon_8.billy")
      end)
    end
  end
end

function map:on_enemy_dead(enemy_name)
  if string.match(enemy_name, "^room_LD1_enemy") and not map:has_entities("room_LD1_enemy") then
    -- LD1 room: kill all enemies will open the doors LD1 and LD2
    if not LD1:is_open() then
      map:open_doors("LD1")
      map:open_doors("LD2")
      sol.audio.play_sound("secret")
    end
  elseif string.match(enemy_name, "^room_LD5_enemy") and not map:has_entities("room_LD5_enemy") and not LD5:is_open() then
    -- LD5 room: kill all enemies will open the door LD5
    map:open_doors("LD5")
    sol.audio.play_sound("secret")
  elseif string.match(enemy_name, "^map_enemy") and not map:has_entities("map_enemy") then
    -- Map chest room: kill all enemies and the chest will appear
    if not map:get_game():get_boolean(700) then
      MAP:set_enabled(true)
      sol.audio.play_sound("chest_appears")
    elseif not LD3:is_open() then
      sol.audio.play_sound("secret")
    end
    map:open_doors("LD3")
  elseif string.match(enemy_name, "^room_big_enemy") and not map:has_entities("room_big_enemy") then
    -- Big key chest room: kill all enemies and the chest will appear
    if not map:get_game():get_boolean(705) then
      BK01:set_enabled(true)
      sol.audio.play_sound("chest_appears")
    end
  end
end

function map:on_obtained_treasure(item_name, variant, savegame_variable)

  if item_name == "heart_container" then
    sol.audio.play_music("victory")
    hero:freeze()
    hero:set_direction(3)
    sol.timer.start(9000, function()
      map:open_doors("boss_door")
      map:open_doors("agahnim_door")
      sol.audio.play_sound("secret")
      hero:unfreeze()
    end)
  end
end

function map:on_dialog_finished(dialog_id)

  if dialog_id == "dungeon_8.billy" then
    sol.audio.play_music("boss")
    hero:unfreeze()
    boss:set_enabled(true)
    billy_npc:set_enabled(false)
  end
end

function map:on_door_open(door_name)

  if door_name == "boss_key_door" then
    boss_key_door_laser:remove()
  elseif door_name == "WW01" then
    sol.audio.play_sound("secret")
  end
end

