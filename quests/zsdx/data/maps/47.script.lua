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

local fighting_boss = false

function map:on_started(destination)
  map:set_doors_open("LD1", true)
  map:set_doors_open("LD3", true)
  map:set_doors_open("LD4", true)
  billy_npc:set_enabled(false)
  map:set_doors_open("boss_door", true)

  -- Hide the map chest if not already opened
  if not map:get_game():get_boolean(700) then
    MAP:set_enabled(false)
  end

  -- Hide the big key chest if not already opened
  if not map:get_game():get_boolean(705) then
    BK01:set_enabled(false)
  end

  -- Link has the mirror shield: no laser obstacles
  if map:get_game():get_ability("shield") >= 3 then
    LO1:set_enabled(false)
    map:set_entities_enabled("LO2", false)
  end

  if destination:get_name() == "from_boss" or destination:get_name() == "from_hidden_room" then
    map:set_doors_open("LD5", true)
  end

  if destination:get_name() == "from_hidden_room" then
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

function map:on_opening_transition_finished(destination)
  if destination:get_name() == "from_outside" then
    map:start_dialog("dungeon_8.welcome")
  end
end

function BB1:on_activated()

  -- LB1 room
  map:move_camera(896, 1712, 250, function()
    LB1:set_enabled(false)
    sol.audio.play_sound("secret")
  end)
end

function BB2:on_activated()

  -- LB2 room
  LB2:set_enabled(false)
  sol.audio.play_sound("secret")
end

function DB4:on_activated()

  map:open_doors("LD4")
  sol.audio.play_sound("secret")
end

function DB06:on_activated()

  -- 4 statues room door opening
  map:open_doors("LD6")
  sol.audio.play_sound("secret")
end

for _, switch in ipairs(map:get_entities("RPS")) do

  function switch:on_activated()
    -- Resets position of statues
    STT1:reset()
    STT2:reset()
    STT3:reset()
    sol.audio.play_sound("warp")
  end
end

function DS1:on_activated()

  -- LD1 room: when Link enters this room, door LD1 closes and enemies appear, sensor DS1 is disabled
  if map:has_entities("room_LD1_enemy") then
    map:close_doors("LD1")
    DS1:set_enabled(false)
  end
end

function DS3:on_activated()

  if map:has_entities("map_enemy") then
    map:close_doors("LD3")
    DS3:set_enabled(false)
  end
end

function DS4:on_activated()

  map:close_doors("LD4")
  DS4:set_enabled(false)
end

function start_boss_sensor:on_activated()

  if not fighting_boss and not map:get_game():get_boolean(727) then
    sol.audio.stop_music()
    map:close_doors("boss_door")
    billy_npc:set_enabled(true)
    hero:freeze()
    fighting_boss = true
    sol.timer.start(1000, function()
      map:start_dialog("dungeon_8.billy", function()
        sol.audio.play_music("boss")
        hero:unfreeze()
        boss:set_enabled(true)
        billy_npc:set_enabled(false)
      end)
    end)
  end
end

for _, enemy in ipairs(map:get_entities("room_LD1_enemy")) do
  
  function enemy:on_dead()
    if not map:has_entities("room_LD1_enemy") then
      -- LD1 room: kill all enemies will open the doors LD1 and LD2
      if not LD1:is_open() then
        map:open_doors("LD1")
        map:open_doors("LD2")
        sol.audio.play_sound("secret")
      end
    end
  end
end

for _, enemy in ipairs(map:get_entities("room_LD5_enemy")) do
  
  function enemy:on_dead()

    if not map:has_entities("room_LD5_enemy") and not LD5:is_open() then
    -- LD5 room: kill all enemies will open the door LD5
    map:open_doors("LD5")
    sol.audio.play_sound("secret")
  end
end

for _, enemy in ipairs(map:get_entities("map_enemy")) do
  
  function enemy:on_dead()

    if not map:has_entities("map_enemy") then
      -- Map chest room: kill all enemies and the chest will appear
      if not map:get_game():get_boolean(700) then
        MAP:set_enabled(true)
        sol.audio.play_sound("chest_appears")
      elseif not LD3:is_open() then
        sol.audio.play_sound("secret")
      end
      map:open_doors("LD3")
    end
  end
end

for _, enemy in ipairs(map:get_entities("room_big_enemy")) do
  
  function enemy:on_dead()

    if not map:has_entities("room_big_enemy") then
      -- Big key chest room: kill all enemies and the chest will appear
      if not map:get_game():get_boolean(705) then
        BK01:set_enabled(true)
        sol.audio.play_sound("chest_appears")
      end
    end
  end
end

function hero:on_obtained_treasure(item_name, variant, savegame_variable)

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

function boss_key_door:on_open()

  boss_key_door_laser:remove()
end

function WW01:on_open()

  sol.audio.play_sound("secret")
end

