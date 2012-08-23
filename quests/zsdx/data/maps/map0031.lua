local map = ...
-- Dungeon 2 B1

-- correct order of the switches
local switches_puzzle_order = {
  switch_a = 1,
  switch_b = 2,
  switch_c = 3,
  switch_d = 4,
  switch_e = 5,
  switch_f = 6
}

local switches_puzzle_nb_enabled = 0
local switches_puzzle_correct = true

-- boss
local boss_arrows = {
  [13] = { x = 864, y = 477, created = true },
  [37] = { x = 896, y = 341, created = true },
  [55] = { x = 1024, y = 341, created = true },
  [80] = { x = 1056, y = 477, created = true },
}
local fighting_boss = false
local timers = {}

function map:on_started(destination_point)

  map:chest_set_enabled("boss_key_chest", false)

  if map:get_game():get_boolean(81) then
    -- boss key chest already found
    for k,v in pairs(switches_puzzle_order) do
      map:switch_set_activated(k, true)
    end
  end

  map:set_doors_open("boss_door", true)
  if destination_point_name == "from_final_room"
      or map:get_game():get_boolean(103) then
    map:set_doors_open("final_room_door", true)
  end

  if map:get_game():get_boolean(103) then
    -- boss heart container already picked
    map:tile_set_enabled("boss_killed_floor", true)
  elseif map:get_game():get_boolean(93) then
    -- boss killed, heart container not picked
    map:create_pickable("heart_container", 1, 103, 960, 437, 0)
  end
end

function map:on_switch_activated(switch_name)

  local order = switches_puzzle_order[switch_name]
  if order ~= nil then 

    switches_puzzle_nb_enabled = switches_puzzle_nb_enabled + 1
    if switches_puzzle_nb_enabled ~= order then
      switches_puzzle_correct = false
    end

    if switches_puzzle_nb_enabled == 6 then

      if switches_puzzle_correct then
	map:move_camera(240, 328, 250, function()
	  map:chest_set_enabled("boss_key_chest", true)
	  sol.audio.play_sound("chest_appears")
	end)
      else
	sol.audio.play_sound("wrong")
	switches_puzzle_nb_enabled = 0
	switches_puzzle_correct = true
	map:switch_set_locked(switch_name, true)
	for k,v in pairs(switches_puzzle_order) do
	  map:switch_set_activated(k, false)
	end
      end
    end
  end
end

function map:on_switch_left(switch_name)

  if switches_puzzle_nb_enabled == 0 then
    for k,v in pairs(switches_puzzle_order) do
      map:switch_set_locked(k, false)
    end
  end
end

function map:on_hero_on_sensor(sensor_name)

  if sensor_name == "start_boss_sensor"
      and not map:get_game():get_boolean(93)
      and not fighting_boss then
    start_boss()

  elseif sensor_name == "close_boss_door_sensor"
      and map:door_is_open("boss_door")
      and not map:get_game():get_boolean(93)
      and not fighting_boss then
    sol.audio.stop_music()
    map:close_doors("boss_door")

  elseif sensor_name == "save_solid_ground_sensor" then
    map:get_hero():save_solid_ground(960, 525, 0)

  elseif sensor_name == "boss_floor_sensor_1" then
    if fighting_boss
      and map:tile_is_enabled("boss_floor_1") then

      map:sensor_set_group_enabled("boss_floor_sensor", false)
      boss_restore_floor(true)
      boss_change_floor(1, 92, 1, false)
      timers[#timers + 1] = sol.timer.start(10000, function()
        map:sensor_set_group_enabled("boss_floor_sensor", true)
	boss_change_floor(92, 1, -1, true)
      end)
    end

  elseif sensor_name == "boss_floor_sensor_2" then
    if fighting_boss
      and map:tile_is_enabled("boss_floor_92") then

      map:sensor_set_group_enabled("boss_floor_sensor", false)
      boss_restore_floor(true)
      boss_change_floor(92, 1, -1, false)
      timers[#timers + 1] = sol.timer.start(10000, function()
        map:sensor_set_group_enabled("boss_floor_sensor", true)
	boss_change_floor(1, 92, 1, true)
      end)
    end
  end
end

function start_boss()

  fighting_boss = true
  map:enemy_set_enabled("boss", true)
  sol.audio.play_music("boss")
end

function map:on_obtained_treasure(item_name, variant, savegame_variable)

  if item_name == "heart_container" then
    sol.timer.start(9000, open_final_room)
    sol.audio.play_music("victory")
    map:get_hero():freeze()
    map:get_hero():set_direction(3)
  end
end

function open_final_room()

  sol.audio.play_sound("secret")
  map:open_doors("final_room_door")
  map:tile_set_enabled("boss_killed_floor", true)
  map:get_hero():unfreeze()
end

function boss_change_floor(first, last, inc, enable)

  local index = first
  local delay
  if enable then
    delay = 30
    for k, v in pairs(boss_arrows) do
      v.created = false
    end
  else
    delay = 75
  end

  function repeat_change()
    if (enable and index % 10 == 0)
      or (not enable and index % 5 == 0) then
      sol.audio.play_sound("stone")
    end
    
    -- enable/disable the tile
    map:tile_set_enabled("boss_floor_" .. index, enable)

    -- create an arrow with some tiles
    if enable and boss_arrows[index] ~= nil then
      map:create_pickable("arrow", 1, -1,
          boss_arrows[index].x, boss_arrows[index].y, 0)
      boss_arrows[index].created = true
    end

    if index ~= last then
      timers[#timers + 1] = sol.timer.start(delay, repeat_change)
    end
    index = index + inc
  end
  repeat_change()
end

function boss_restore_floor(with_arrows)

  -- restore the whole floor immediately
  map:tile_set_group_enabled("boss_floor", true)
  for _, t in ipairs(timers) do t:stop() end

  if with_arrows then
    for k, v in pairs(boss_arrows) do
      if not v.created then
        map:create_pickable("arrow", 1, -1, v.x, v.y, 0)
        v.created = true
      end
    end
  end
end

function map:on_enemy_dying(enemy_name)

  if enemy_name == "boss" then
    boss_restore_floor(false)
    map:sensor_set_group_enabled("boss_floor_sensor", false)
  end
end

function map:on_enemy_dead(enemy_name)

  if enemy_name == "boss" then
    -- create the heart container manually to be sure it won't be in lava
    map:create_pickable("heart_container", 1, 103, 960, 437, 0)
    for _, t in ipairs(timers) do t:stop() end
  end
end

