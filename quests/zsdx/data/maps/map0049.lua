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

function map:on_started(destination_point)
  if not map:get_game():get_boolean(711) then
    RC100:set_enabled(false)
  end
  if destination_point:get_name() == "from_B3_C" then
    map:set_doors_open("LD15", true)
    for i = 1, 9 do
      map:get_entity("BRoom" .. i):set_activated(true)
    end
  end
end

function button_room_test_combination()
  if BRoom1:is_activated()
    and BRoom2:is_activated()
    and BRoom3:is_activated()
    and BRoom4:is_activated()
    and BRoom5:is_activated()
    and BRoom6:is_activated()
    and BRoom7:is_activated()
    and BRoom8:is_activated()
    and BRoom9:is_activated() then
    if not LD15:is_open() then
      sol.audio.play_sound("secret")
      map:open_doors("LD15")
      BRoomReset:set_activated(true)
    end
  end
end

function map:on_switch_activated(switch_name)
  if switch_name == "BRoom1" then
    BRoom2:set_activated(not BRoom2:is_activated())
    BRoom4:set_activated(not BRoom4:is_activated())
    BRoomReset:set_activated(false)
    button_room_test_combination()
  elseif switch_name == "BRoom2" then
    BRoom1:set_activated(not BRoom1:is_activated())
    BRoom3:set_activated(not BRoom3:is_activated())
    BRoom5:set_activated(not BRoom5:is_activated())
    BRoomReset:set_activated(false)
    button_room_test_combination()
  elseif switch_name == "BRoom3" then
    BRoom2:set_activated(not BRoom2:is_activated())
    BRoom6:set_activated(not BRoom6:is_activated())
    BRoomReset:set_activated(false)
    button_room_test_combination()
  elseif switch_name == "BRoom4" then
    BRoom1:set_activated(not BRoom1:is_activated())
    BRoom5:set_activated(not BRoom5:is_activated())
    BRoom7:set_activated(not BRoom7:is_activated())
    BRoomReset:set_activated(false)
    button_room_test_combination()
  elseif switch_name == "BRoom5" then
    BRoom2:set_activated(not BRoom2:is_activated())
    BRoom4:set_activated(not BRoom4:is_activated())
    BRoom6:set_activated(not BRoom6:is_activated())
    BRoom8:set_activated(not BRoom8:is_activated())
    BRoomReset:set_activated(false)
    button_room_test_combination()
  elseif switch_name == "BRoom6" then
    BRoom3:set_activated(not BRoom3:is_activated())
    BRoom5:set_activated(not BRoom5:is_activated())
    BRoom9:set_activated(not BRoom9:is_activated())
    BRoomReset:set_activated(false)
    button_room_test_combination()
  elseif switch_name == "BRoom7" then
    BRoom4:set_activated(not BRoom4:is_activated())
    BRoom8:set_activated(not BRoom8:is_activated())
    BRoomReset:set_activated(false)
    button_room_test_combination()
  elseif switch_name == "BRoom8" then
    BRoom5:set_activated(not BRoom5:is_activated())
    BRoom7:set_activated(not BRoom7:is_activated())
    BRoom9:set_activated(not BRoom9:is_activated())
    BRoomReset:set_activated(false)
    button_room_test_combination()
  elseif switch_name == "BRoom9" then
    BRoom6:set_activated(not BRoom6:is_activated())
    BRoom8:set_activated(not BRoom8:is_activated())
    BRoomReset:set_activated(false)
    button_room_test_combination()
  elseif switch_name == "BRoomReset" then
    BRoom1:set_activated(false)
    BRoom2:set_activated(false)
    BRoom3:set_activated(false)
    BRoom4:set_activated(false)
    BRoom5:set_activated(false)
    BRoom6:set_activated(false)
    BRoom7:set_activated(false)
    BRoom8:set_activated(false)
    BRoom9:set_activated(false)
  elseif switch_name == "DB16" then
    map:move_camera(808, 1056, 250, DB16_open_door)
  end
end

function DB16_open_door()
  map:open_doors("LD16")
end

function DB16_time_out()
  map:close_doors("LD16")
  DB16:set_activated(false)
end

function map:on_camera_back()
  timer = sol.timer.start(DB16_time_out, 10000, true)
end

function map:on_npc_interaction(npc_name)
  if npc_name == "TD01" then
    sol.audio.play_sound("warp")
    hero_x, hero_y = hero:get_position()
    hero:set_position(hero_x - 320, hero_y)
  elseif npc_name == "TD02" then
    sol.audio.play_sound("warp")
    hero_x, hero_y = hero:get_position()
    hero:set_position(hero_x + 320, hero_y)
  end
end

function map:on_enemy_dead(enemy_name)
  if string.find(enemy_name, "^rupee_enemy") and not map:has_entities("rupee_enemy") then
    -- 100 rupee room: kill all enemies
    if not RC100:is_enabled() then
      sol.audio.play_sound("chest_appears")
      RC100:set_enabled(true)
    end
  end
end

function map:on_hero_on_sensor(sensor_name)

  if sensor_name == "dont_close_LD16_sensor" then
    if timer ~= nil then
      timer:stop()
      timer = nil
    end
    map:get_entity(sensor_name):set_enabled(false)
  end
end

