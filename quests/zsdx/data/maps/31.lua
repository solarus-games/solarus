local map = ...
-- Dungeon 2 B1

-- correct order of the switches
local switches_puzzle_order = {
  [switch_a] = 1,
  [switch_b] = 2,
  [switch_c] = 3,
  [switch_d] = 4,
  [switch_e] = 5,
  [switch_f] = 6
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

local function puzzle_switch_activated(switch)

  local order = switches_puzzle_order[switch]
  if order ~= nil then

    switches_puzzle_nb_enabled = switches_puzzle_nb_enabled + 1
    if switches_puzzle_nb_enabled ~= order then
      switches_puzzle_correct = false
    end

    if switches_puzzle_nb_enabled == 6 then

      if switches_puzzle_correct then
	map:move_camera(240, 328, 250, function()
	  boss_key_chest:set_enabled(true)
	  sol.audio.play_sound("chest_appears")
	end)
      else
	sol.audio.play_sound("wrong")
	switches_puzzle_nb_enabled = 0
	switches_puzzle_correct = true
	switch:set_locked(true)
	for sw, _ in pairs(switches_puzzle_order) do
	  sw:set_activated(false)
	end
      end
    end
  end
end

local function puzzle_switch_left(switch)

  if switches_puzzle_nb_enabled == 0 then
    for switch, _ in pairs(switches_puzzle_order) do
      switch:set_locked(false)
    end
  end
end

local function boss_change_floor(first, last, inc, enable)

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

  local function repeat_change()
    if (enable and index % 10 == 0)
      or (not enable and index % 5 == 0) then
      sol.audio.play_sound("stone")
    end

    -- enable/disable the tile
    map:get_entity("boss_floor_" .. index):set_enabled(enable)

    -- create an arrow with some tiles
    if enable and boss_arrows[index] ~= nil then
      map:create_pickable{
	treasure_name = "arrow",
	treasure_variant = 1,
	x = boss_arrows[index].x,
	y = boss_arrows[index].y,
	layer = 0
      }
      boss_arrows[index].created = true
    end

    if index ~= last then
      timers[#timers + 1] = sol.timer.start(delay, repeat_change)
    end
    index = index + inc
  end
  repeat_change()
end

local function boss_restore_floor(with_arrows)

  -- restore the whole floor immediately
  map:set_entities_enabled("boss_floor", true)
  for _, t in ipairs(timers) do t:stop() end

  if with_arrows then
    for k, v in pairs(boss_arrows) do
      if not v.created then
        map:create_pickable{
	  treasure_name = "arrow",
	  treasure_variant = 1,
	  x = v.x,
	  y = v.y,
	  layer = 0
	}
        v.created = true
      end
    end
  end
end

function map:on_started(destination)

  boss_key_chest:set_enabled(false)

  if map:get_game():get_value("b81") then
    -- boss key chest already found
    for switch, _ in pairs(switches_puzzle_order) do
      switch:set_activated(true)
      switch.on_activated = puzzle_switch_activated
      switch.on_left = puzzle_switch_left
    end
  end

  map:set_doors_open("boss_door", true)
  if destination:get_name() == "from_final_room"
      or map:get_game():get_value("b103") then
    map:set_doors_open("final_room_door", true)
  end

  if map:get_game():get_value("b103") then
    -- boss heart container already picked
    boss_killed_floor:set_enabled(true)
  elseif map:get_game():get_value("b93") then
    -- boss killed, heart container not picked
    map:create_pickable{
      treasure_name = "heart_container",
      treasure_variant = 1,
      treasure_savegame_variable = "b103",
      x = 960,
      y = 437,
      layer = 0
    }
  end
end

function start_boss_sensor:on_activated()

  if not map:get_game():get_value("b93")
      and not fighting_boss then
    fighting_boss = true
    boss:set_enabled(true)
    sol.audio.play_music("boss")
  end
end

function close_boss_door_sensor:on_activated()

  if boss_door:is_open()
      and not map:get_game():get_value("b93")
      and not fighting_boss then
    sol.audio.stop_music()
    map:close_doors("boss_door")
  end
end

function save_solid_ground_sensor:on_activated()
  hero:save_solid_ground(960, 525, 0)
end

function boss_floor_sensor_1:on_activated()

  if fighting_boss
      and boss_floor_1:is_enabled() then

    map:set_entities_enabled("boss_floor_sensor", false)
    boss_restore_floor(true)
    boss_change_floor(1, 92, 1, false)
    timers[#timers + 1] = sol.timer.start(10000, function()
      map:set_entities_enabled("boss_floor_sensor", true)
      boss_change_floor(92, 1, -1, true)
    end)
  end
end

function boss_floor_sensor_2:on_activated()

  if fighting_boss
      and boss_floor_92:is_enabled() then

    map:set_entities_enabled("boss_floor_sensor", false)
    boss_restore_floor(true)
    boss_change_floor(92, 1, -1, false)
    timers[#timers + 1] = sol.timer.start(10000, function()
      map:set_entities_enabled("boss_floor_sensor", true)
      boss_change_floor(1, 92, 1, true)
    end)
  end
end

function map:on_obtained_treasure(item, variant, savegame_variable)

  if item:get_name() == "heart_container" then
    sol.timer.start(9000, function()
      sol.audio.play_sound("secret")
      map:open_doors("final_room_door")
      boss_killed_floor:set_enabled(true)
      self:unfreeze()
    end)
    sol.audio.play_music("victory")
    self:freeze()
    self:set_direction(3)
  end
end

function boss:on_dying()

  boss_restore_floor(false)
  map:set_entities_enabled("boss_floor_sensor", false)
end

if boss ~= nil then
  function boss:on_dead()

    -- create the heart container manually to be sure it won't be in lava
    map:create_pickable{
      treasure_name = "heart_container",
      treasure_variant = 1,
      treasure_savegame_variable = "b103",
      x = 960,
      y = 437,
      layer = 0
    }
    for _, t in ipairs(timers) do t:stop() end
  end
end

