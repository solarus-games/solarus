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

function map:on_started(destination_point_name)
  map:set_doors_open("LD9", true)

  -- Link has mirror shield: no laser obstacles
  if map:get_game():get_ability("shield") >= 3 then
    map:wall_set_enabled("LO4", false)
  end

  if map:get_game():get_boolean(706) then
    map:switch_set_activated("CB03", true)
  else
    map:chest_set_enabled("KC03", false)
  end
  if not map:get_game():get_boolean(707) then
    map:chest_set_enabled("KC04", false)
  end

  if destination_point_name == "from_1F_A" then
    map:set_doors_open("LD8", true)
    map:switch_set_activated("DB08", true)
  end

  if destination_point_name ~= "from_B2_C" then
    map:set_doors_open("LD12", true)
  end
end

function map:on_hero_on_sensor(sensor_name)
  if sensor_name == "DS12" then
    -- Push block room		
    if not map:door_is_open("LD12") then
      sol.audio.play_sound("secret")
      map:open_doors("LD12")
      map:sensor_set_enabled("DS12", false)
    end
  elseif sensor_name == "DS7" then
    -- Globules monsters room		
    if map:door_is_open("LD7")
        and map:has_entities("LD7_enemy") then		
      map:close_doors("LD7")
    end
  elseif sensor_name == "DS9" then
    -- Hard hat beetles room		
    if map:door_is_open("LD9")
        and map:has_entities("LD9_enemy") then		
      map:close_doors("LD9")
      map:sensor_set_enabled("DS9", false)
    end
  end
end

function map:on_switch_activated(switch_name)
  if switch_name == "CB03" then
    if not map:get_game():get_boolean(706) then
      map:move_camera(1488, 1152, 250, CB03_chest_appears)
    end
  elseif switch_name == "CB04" then
    map:chest_set_enabled("KC04", true)
    sol.audio.play_sound("chest_appears")
  elseif string.match(switch_name, "^DB08") then
    sol.audio.play_sound("secret")
    map:open_doors("LD8")
    map:open_doors("LD7")
  end
end

function CB03_chest_appears()
  map:chest_set_enabled("KC03", true)
  sol.audio.play_sound("chest_appears")
end

function CB03_time_out()
  if not map:chest_is_open("KC03") then
    sol.audio.play_sound("door_closed")
    map:chest_set_enabled("KC03", false)
    map:switch_set_activated("CB03", false)
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
    if not map:door_is_open("LD7") then
      map:open_doors("LD7")
      sol.audio.play_sound("secret")
    end
  elseif string.match(enemy_name, "^LD9_enemy") and not map:has_entities("LD9_enemy") then	
    -- LD9 room: kill all enemies will open the door LD9
    if not map:door_is_open("LD9") then
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

