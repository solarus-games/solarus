local map = ...
-- Dungeon 8 B1

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
  map:set_doors_open("LD9", true)

  -- Link has mirror shield: no laser obstacles
  if map:get_game():get_ability("shield") >= 3 then
    LO4:set_enabled(false)
  end

  if map:get_game():get_boolean(706) then
    CB03:set_activated(true)
  else
    KC03:set_enabled(false)
  end
  if not map:get_game():get_boolean(707) then
    KC04:set_enabled(false)
  end

  if destination_point:get_name() == "from_1F_A" then
    map:set_doors_open("LD8", true)
    DB08:set_activated(true)
  end

  if destination_point:get_name() ~= "from_B2_C" then
    map:set_doors_open("LD12", true)
  end
end

function map:on_hero_on_sensor(sensor_name)
  if sensor_name == "DS12" then
    -- Push block room		
    if not LD12:is_open() then
      sol.audio.play_sound("secret")
      map:open_doors("LD12")
      DS12:set_enabled(false)
    end
  elseif sensor_name == "DS7" then
    -- Globules monsters room		
    if LD7:is_open()
        and map:has_entities("LD7_enemy") then		
      map:close_doors("LD7")
    end
  elseif sensor_name == "DS9" then
    -- Hard hat beetles room		
    if LD9:is_open()
        and map:has_entities("LD9_enemy") then		
      map:close_doors("LD9")
      DS9:set_enabled(false)
    end
  end
end

function map:on_switch_activated(switch_name)
  if switch_name == "CB03" then
    if not map:get_game():get_boolean(706) then
      map:move_camera(1488, 1152, 250, CB03_chest_appears)
    end
  elseif switch_name == "CB04" then
    KC04:set_enabled(true)
    sol.audio.play_sound("chest_appears")
  elseif string.match(switch_name, "^DB08") then
    sol.audio.play_sound("secret")
    map:open_doors("LD8")
    map:open_doors("LD7")
  end
end

function CB03_chest_appears()
  KC03:set_enabled(true)
  sol.audio.play_sound("chest_appears")
end

function CB03_time_out()
  if not KC03:is_open() then
    sol.audio.play_sound("door_closed")
    KC03:set_enabled(false)
    CB03:set_activated(false)
  end
end

function map:on_camera_back()
  timer = sol.timer.start(8000, true, CB03_time_out)
end

function map:on_obtaining_treasure(item_name, variant, savegame_variable)

  if savegame_variable == 706 then
    if timer ~= nil then
      timer:stop()
      timer = nil
    end
  end
end


function map:on_enemy_dead(enemy_name)
  if string.match(enemy_name, "^LD7_enemy") and not map:has_entities("LD7_enemy") then	
    -- LD7 room: kill all enemies will open the door LD7
    if not LD7:is_open() then
      map:open_doors("LD7")
      sol.audio.play_sound("secret")
    end
  elseif string.match(enemy_name, "^LD9_enemy") and not map:has_entities("LD9_enemy") then	
    -- LD9 room: kill all enemies will open the door LD9
    if not LD9:is_open() then
      map:open_doors("LD9")
      sol.audio.play_sound("secret")
    end
  end
end

function map:on_door_open(door_name)

  if door_name == "WW02" then
    sol.audio.play_sound("secret")
  end
end

