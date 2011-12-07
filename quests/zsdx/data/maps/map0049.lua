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

function event_map_started(destination_point_name)
  sol.map.door_set_open("LD11", true)
  if not sol.game.savegame_get_boolean(711) then
    sol.map.chest_set_enabled("RC100", false)
  end
  if destination_point_name == "from_B3_C" then
    sol.map.door_set_open("LD15", true)
    for i = 1, 9 do
      sol.map.switch_set_activated("BRoom" .. i, true)
    end
  end
end

function button_room_test_combination()
  if sol.map.switch_is_activated("BRoom1")
    and sol.map.switch_is_activated("BRoom2")
    and sol.map.switch_is_activated("BRoom3")
    and sol.map.switch_is_activated("BRoom4")
    and sol.map.switch_is_activated("BRoom5")
    and sol.map.switch_is_activated("BRoom6")
    and sol.map.switch_is_activated("BRoom7")
    and sol.map.switch_is_activated("BRoom8")
    and sol.map.switch_is_activated("BRoom9") then
    if not sol.map.door_is_open("LD15") then
      sol.main.play_sound("secret")
      sol.map.door_open("LD15")
      sol.map.switch_set_activated("BRoomReset", true)
    end
  end
end

function event_switch_activated(switch_name)
  if switch_name == "BRoom1" then
    sol.map.switch_set_activated("BRoom2", not sol.map.switch_is_activated("BRoom2"))
    sol.map.switch_set_activated("BRoom4", not sol.map.switch_is_activated("BRoom4"))
    sol.map.switch_set_activated("BRoomReset", false)
    button_room_test_combination()
  elseif switch_name == "BRoom2" then
    sol.map.switch_set_activated("BRoom1", not sol.map.switch_is_activated("BRoom1"))
    sol.map.switch_set_activated("BRoom3", not sol.map.switch_is_activated("BRoom3"))
    sol.map.switch_set_activated("BRoom5", not sol.map.switch_is_activated("BRoom5"))
    sol.map.switch_set_activated("BRoomReset", false)
    button_room_test_combination()
  elseif switch_name == "BRoom3" then
    sol.map.switch_set_activated("BRoom2", not sol.map.switch_is_activated("BRoom2"))
    sol.map.switch_set_activated("BRoom6", not sol.map.switch_is_activated("BRoom6"))
    sol.map.switch_set_activated("BRoomReset", false)
    button_room_test_combination()
  elseif switch_name == "BRoom4" then
    sol.map.switch_set_activated("BRoom1", not sol.map.switch_is_activated("BRoom1"))
    sol.map.switch_set_activated("BRoom5", not sol.map.switch_is_activated("BRoom5"))
    sol.map.switch_set_activated("BRoom7", not sol.map.switch_is_activated("BRoom7"))
    sol.map.switch_set_activated("BRoomReset", false)
    button_room_test_combination()
  elseif switch_name == "BRoom5" then
    sol.map.switch_set_activated("BRoom2", not sol.map.switch_is_activated("BRoom2"))
    sol.map.switch_set_activated("BRoom4", not sol.map.switch_is_activated("BRoom4"))
    sol.map.switch_set_activated("BRoom6", not sol.map.switch_is_activated("BRoom6"))
    sol.map.switch_set_activated("BRoom8", not sol.map.switch_is_activated("BRoom8"))
    sol.map.switch_set_activated("BRoomReset", false)
    button_room_test_combination()
  elseif switch_name == "BRoom6" then
    sol.map.switch_set_activated("BRoom3", not sol.map.switch_is_activated("BRoom3"))
    sol.map.switch_set_activated("BRoom5", not sol.map.switch_is_activated("BRoom5"))
    sol.map.switch_set_activated("BRoom9", not sol.map.switch_is_activated("BRoom9"))
    sol.map.switch_set_activated("BRoomReset", false)
    button_room_test_combination()
  elseif switch_name == "BRoom7" then
    sol.map.switch_set_activated("BRoom4", not sol.map.switch_is_activated("BRoom4"))
    sol.map.switch_set_activated("BRoom8", not sol.map.switch_is_activated("BRoom8"))
    sol.map.switch_set_activated("BRoomReset", false)
    button_room_test_combination()
  elseif switch_name == "BRoom8" then
    sol.map.switch_set_activated("BRoom5", not sol.map.switch_is_activated("BRoom5"))
    sol.map.switch_set_activated("BRoom7", not sol.map.switch_is_activated("BRoom7"))
    sol.map.switch_set_activated("BRoom9", not sol.map.switch_is_activated("BRoom9"))
    sol.map.switch_set_activated("BRoomReset", false)
    button_room_test_combination()
  elseif switch_name == "BRoom9" then
    sol.map.switch_set_activated("BRoom6", not sol.map.switch_is_activated("BRoom6"))
    sol.map.switch_set_activated("BRoom8", not sol.map.switch_is_activated("BRoom8"))
    sol.map.switch_set_activated("BRoomReset", false)
    button_room_test_combination()
  elseif switch_name == "BRoomReset" then
    sol.map.switch_set_activated("BRoom1", false)
    sol.map.switch_set_activated("BRoom2", false)
    sol.map.switch_set_activated("BRoom3", false)
    sol.map.switch_set_activated("BRoom4", false)
    sol.map.switch_set_activated("BRoom5", false)
    sol.map.switch_set_activated("BRoom6", false)
    sol.map.switch_set_activated("BRoom7", false)
    sol.map.switch_set_activated("BRoom8", false)
    sol.map.switch_set_activated("BRoom9", false)
  elseif switch_name == "DB16" then
    sol.map.camera_move(808, 1056, 250, DB16_open_door)
  end
end

function DB16_open_door()
  sol.map.door_open("LD16")
end

function DB16_time_out()
  sol.map.door_close("LD16")
  sol.map.switch_set_activated("DB16", false)
end

function event_camera_back()
  sol.main.timer_start(DB16_time_out, 10000, true)
end

function event_npc_interaction(npc_name)
  if npc_name == "TD01" then
    sol.main.play_sound("warp")
    hero_x, hero_y = sol.map.hero_get_position()
    sol.map.hero_set_position(hero_x - 320, hero_y)
  elseif npc_name == "TD02" then
    sol.main.play_sound("warp")
    hero_x, hero_y = sol.map.hero_get_position()
    sol.map.hero_set_position(hero_x + 320, hero_y)
  end
end

function event_enemy_dead(enemy_name)
  if string.find(enemy_name, "^rupee_enemy") and sol.map.enemy_is_group_dead("rupee_enemy") then	
    -- 100 rupee room: kill all enemies
    if not sol.map.chest_is_enabled("RC100") then
      sol.main.play_sound("chest_appears")
      sol.map.chest_set_enabled("RC100", true)
    end
  end
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "dont_close_LD16_sensor" then
    sol.main.timer_stop_all()
    sol.map.sensor_set_enabled(sensor_name, false)
  end
end

