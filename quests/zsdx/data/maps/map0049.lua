local map = ...
-- Dungeon 8 B2

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

local timer

function map:on_started(destination_point_name)
  if not map:get_game():get_boolean(711) then
    map:chest_set_enabled("RC100", false)
  end
  if destination_point_name == "from_B3_C" then
    map:door_set_open("LD15", true)
    for i = 1, 9 do
      map:switch_set_activated("BRoom" .. i, true)
    end
  end
end

function button_room_test_combination()
  if map:switch_is_activated("BRoom1")
    and map:switch_is_activated("BRoom2")
    and map:switch_is_activated("BRoom3")
    and map:switch_is_activated("BRoom4")
    and map:switch_is_activated("BRoom5")
    and map:switch_is_activated("BRoom6")
    and map:switch_is_activated("BRoom7")
    and map:switch_is_activated("BRoom8")
    and map:switch_is_activated("BRoom9") then
    if not map:door_is_open("LD15") then
      sol.audio.play_sound("secret")
      map:door_open("LD15")
      map:switch_set_activated("BRoomReset", true)
    end
  end
end

function map:on_switch_activated(switch_name)
  if switch_name == "BRoom1" then
    map:switch_set_activated("BRoom2", not map:switch_is_activated("BRoom2"))
    map:switch_set_activated("BRoom4", not map:switch_is_activated("BRoom4"))
    map:switch_set_activated("BRoomReset", false)
    button_room_test_combination()
  elseif switch_name == "BRoom2" then
    map:switch_set_activated("BRoom1", not map:switch_is_activated("BRoom1"))
    map:switch_set_activated("BRoom3", not map:switch_is_activated("BRoom3"))
    map:switch_set_activated("BRoom5", not map:switch_is_activated("BRoom5"))
    map:switch_set_activated("BRoomReset", false)
    button_room_test_combination()
  elseif switch_name == "BRoom3" then
    map:switch_set_activated("BRoom2", not map:switch_is_activated("BRoom2"))
    map:switch_set_activated("BRoom6", not map:switch_is_activated("BRoom6"))
    map:switch_set_activated("BRoomReset", false)
    button_room_test_combination()
  elseif switch_name == "BRoom4" then
    map:switch_set_activated("BRoom1", not map:switch_is_activated("BRoom1"))
    map:switch_set_activated("BRoom5", not map:switch_is_activated("BRoom5"))
    map:switch_set_activated("BRoom7", not map:switch_is_activated("BRoom7"))
    map:switch_set_activated("BRoomReset", false)
    button_room_test_combination()
  elseif switch_name == "BRoom5" then
    map:switch_set_activated("BRoom2", not map:switch_is_activated("BRoom2"))
    map:switch_set_activated("BRoom4", not map:switch_is_activated("BRoom4"))
    map:switch_set_activated("BRoom6", not map:switch_is_activated("BRoom6"))
    map:switch_set_activated("BRoom8", not map:switch_is_activated("BRoom8"))
    map:switch_set_activated("BRoomReset", false)
    button_room_test_combination()
  elseif switch_name == "BRoom6" then
    map:switch_set_activated("BRoom3", not map:switch_is_activated("BRoom3"))
    map:switch_set_activated("BRoom5", not map:switch_is_activated("BRoom5"))
    map:switch_set_activated("BRoom9", not map:switch_is_activated("BRoom9"))
    map:switch_set_activated("BRoomReset", false)
    button_room_test_combination()
  elseif switch_name == "BRoom7" then
    map:switch_set_activated("BRoom4", not map:switch_is_activated("BRoom4"))
    map:switch_set_activated("BRoom8", not map:switch_is_activated("BRoom8"))
    map:switch_set_activated("BRoomReset", false)
    button_room_test_combination()
  elseif switch_name == "BRoom8" then
    map:switch_set_activated("BRoom5", not map:switch_is_activated("BRoom5"))
    map:switch_set_activated("BRoom7", not map:switch_is_activated("BRoom7"))
    map:switch_set_activated("BRoom9", not map:switch_is_activated("BRoom9"))
    map:switch_set_activated("BRoomReset", false)
    button_room_test_combination()
  elseif switch_name == "BRoom9" then
    map:switch_set_activated("BRoom6", not map:switch_is_activated("BRoom6"))
    map:switch_set_activated("BRoom8", not map:switch_is_activated("BRoom8"))
    map:switch_set_activated("BRoomReset", false)
    button_room_test_combination()
  elseif switch_name == "BRoomReset" then
    map:switch_set_activated("BRoom1", false)
    map:switch_set_activated("BRoom2", false)
    map:switch_set_activated("BRoom3", false)
    map:switch_set_activated("BRoom4", false)
    map:switch_set_activated("BRoom5", false)
    map:switch_set_activated("BRoom6", false)
    map:switch_set_activated("BRoom7", false)
    map:switch_set_activated("BRoom8", false)
    map:switch_set_activated("BRoom9", false)
  elseif switch_name == "DB16" then
    map:camera_move(808, 1056, 250, DB16_open_door)
  end
end

function DB16_open_door()
  map:door_open("LD16")
end

function DB16_time_out()
  map:door_close("LD16")
  map:switch_set_activated("DB16", false)
end

function map:on_camera_back()
  timer = sol.timer.start(DB16_time_out, 10000, true)
end

function map:on_npc_interaction(npc_name)
  if npc_name == "TD01" then
    sol.audio.play_sound("warp")
    hero_x, hero_y = map:hero_get_position()
    map:hero_set_position(hero_x - 320, hero_y)
  elseif npc_name == "TD02" then
    sol.audio.play_sound("warp")
    hero_x, hero_y = map:hero_get_position()
    map:hero_set_position(hero_x + 320, hero_y)
  end
end

function map:on_enemy_dead(enemy_name)
  if string.find(enemy_name, "^rupee_enemy") and map:enemy_is_group_dead("rupee_enemy") then	
    -- 100 rupee room: kill all enemies
    if not map:chest_is_enabled("RC100") then
      sol.audio.play_sound("chest_appears")
      map:chest_set_enabled("RC100", true)
    end
  end
end

function map:on_hero_on_sensor(sensor_name)

  if sensor_name == "dont_close_LD16_sensor" then
    if timer ~= nil then
      timer:stop()
      timer = nil
    end
    map:sensor_set_enabled(sensor_name, false)
  end
end

