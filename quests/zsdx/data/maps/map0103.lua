local map = ...
-- Dungeon 7 3F

local fighting_boss = false
local just_removed_special_torch = false
local special_torch_timer = nil

local door_properties = { -- properties of the 5 timed doors
  [door_a] = { x = 920, y = 640, delay = 12000 },
  [door_b] = { x = 864, y = 808, delay = 15000 },
  [door_c] = { x = 1024, y = 840, delay = 12000 },
  [door_d] = { x = 832, y = 936, delay = 4000 },
  [door_e] = { x = 976, y = 952, delay = 4000 }
}
local current_door = nil -- current door during a camera movement
local door_timers = {} -- doors that currently have a timer running
local arrows_timer

function map:on_started(destination_point)

  -- block fallen into the hole
  if map:get_game():get_boolean(623) then
    nw_block:set_enabled(false)
  end

  -- NW door
  if map:get_game():get_boolean(624) then
    map:set_doors_open("ne_door", true)
  end

  -- door A (timed doors)
  if map:get_game():get_boolean(627) then
    door_a_switch:set_activated(true)
  end

  -- boss
  map:set_doors_open("boss_door", true)
  if map:get_game():get_boolean(625)
    and not map:get_game():get_boolean(626) then
    -- boss killed, heart container not picked
    map:create_pickable("heart_container", 1, 626, 544, 789, 0)
  end

  -- special torch door
  if map:get_game():get_boolean(624) then
    ne_switch:set_activated(true)
  end
end

function nw_block:on_moved()

  local x, y = self:get_position()
  if x == 544 and y == 69 then
    -- make the block fall
    self:set_enabled(false)
    hole_a:set_enabled(true)
    hole_a_teletransporter:set_enabled(true)
    map:get_game():set_boolean(623, true)
    sol.audio.play_sound("jump")
    sol.timer.start(500, function() sol.audio.play_sound("bomb") end)
  end
end

function map:on_update()

  if not map:get_game():get_boolean(623) then
    -- blocks could overlap holes or teletransporters with old versions of the engine,
    -- so we disable the hole A and its teletransporter when necessary
    -- TODO this is not necessary anymore

    local disable_hole = false
    local block_x, block_y = nw_block:get_position()
    if block_y == 69 and block_x >= 520 and block_x < 544 then
      -- the block is just before the hole
      local hero_x = hero:get_position()
      disable_hole = hero_x < block_x
    end

    if disable_hole and hole_a:is_enabled() then
      hole_a:set_enabled(false)
      hole_a_teletransporter:set_enabled(false)
    elseif not disable_hole and not hole_a:is_enabled() then
      hole_a:set_enabled(true)
      hole_a_teletransporter:set_enabled(true)
    end
  end
end

-- north-east room
function ne_switch:on_activated()

  current_door = nil
  map:move_camera(960, 312, 250, function()
    sol.audio.play_sound("secret")
    map:open_doors("ne_door")
  end)
end

-- switch that removes the special torch
function special_torch_switch:on_activated()

  current_door = nil
  map:move_camera(960, 120, 250, function()
    sol.audio.play_sound("secret")
    special_torch:set_enabled(false)
    just_removed_special_torch = true
  end)
end

-- timed doors
for door, _ in pairs(door_properties) do
  local switch = map:get_entity(door:get_name() .. "_switch")
  local done_sensor = map:get_entity(door:get_name() .. "_done_sensor")
  local close_sensor = map:get_entity(door:get_name() .. "_close_sensor")
  switch.on_activated = timed_door_switch_activated
  if done_sensor ~= nil then
    done_sensor.on_activated = timer_door_done_sensor_activated
  end
  if close_sensor ~= nil then
    close_sensor.on_activated = timer_door_close_sensor_activated
  end
end

local function timed_door_switch_activated(switch)

  local door_name = switch:get_name():match("^(door_[a-e])_switch$")
  current_door = map:get_entity(door_name)
  if current_door ~= nil then
    local properties = door_properties[current_door]
    map:move_camera(properties.x, properties.y, 250, function()
      map:open_doors(door_name)
    end)
  end
end

function map:on_camera_back()

  -- set up a timer when the camera movement is finished
  if just_removed_special_torch then
    just_removed_special_torch = false
    special_torch_timer = sol.timer.start(8000, function()
      sol.audio.play_sound("door_closed")
      special_torch:set_enabled(true)
      special_torch_switch:set_activated(false)
      special_torch_timer = nil
    end)
    special_torch_timer:set_with_sound(true)

  elseif current_door ~= nil then
    local door_name = current_door:get_name()
    local timer = sol.timer.start(door_properties[current_door].delay, function()
      if door_timers[current_door] ~= nil then
	map:close_doors(door_name)
	map:get_entity(door_name .. "_switch"):set_activated(false)
	door_timers[current_door] = nil
      end
    end)
    timer:set_with_sound(true)
    door_timers[current_door] = true
    current_door = nil

  end
end

-- special torch
function special_torch_dont_close_sensor:on_activated()

  if special_torch_timer ~= nil then
    special_torch_timer:stop()
    special_torch_timer = nil
  end
end

-- boss door
function close_boss_door_sensor:on_activated()

  if boss_door:is_open() then
    map:close_doors("boss_door")
    sol.audio.stop_music()
  end
end

-- boss
function start_boss_sensor:on_activated()

  if not map:get_game():get_boolean(625)
      and not fighting_boss then
    start_boss()
  end
end

-- west room
function w_room_sensor:on_activated()

  sol.audio.play_sound("secret")
  local state = w_room_tile_1:is_enabled()
  w_room_tile_1:set_enabled(not state)
  w_room_tile_2:set_enabled(state)
end
w_room_sensor_2.on_activated = w_room_sensor.on_activated

-- pass a timed door
local function timed_door_done_sensor_activated(sensor)

  local door_name = sensor:get_name():match("^(door_[a-e])_done_sensor$")
  local door = map:get_entity(door_name)
  if door ~= nil then
    door_timers[door] = nil -- disable the timer
  end
end

-- close a timed door previously passed (i.e. it has no current timer)
local function timed_door_close_sensor_activated(sensor)

  local door_name = sensor:get_name():match("^(door_[a-e])_close_sensor$")
  local door = map:get_entity(door_name)

  if door ~= nil then
    if door_timers[door] == nil and door:is_open() then
      map:close_doors(door_name)
      map:get_entity(door_name .. "_switch"):set_activated(false)
    end
  end
end

local function start_boss()

  sol.audio.play_music("boss")
  boss:set_enabled(true)
  fighting_boss = true

  arrows_timer = sol.timer.start(20000, repeat_give_arrows)
end

local function repeat_give_arrows()

  -- give arrows if necessary during the boss fight
  if map:get_game():get_item_amount("bow") == 0 then
    local positions = {
      { x = 416, y = 685 },
      { x = 672, y = 685 },
      { x = 416, y = 885 },
      { x = 672, y = 885 },
    }
    arrow_xy = positions[math.random(#positions)]
    map:create_pickable("arrow", 3, -1, arrow_xy.x, arrow_xy.y, 0)
  end
  arrows_timer = sol.timer.start(20000, repeat_give_arrows)
end

function hero:on_obtained_treasure(item_name, variant, savegame_variable)

  if item_name == "heart_container" then
    sol.audio.play_music("victory")
    hero:freeze()
    hero:set_direction(3)
    sol.timer.start(9000, function()
      sol.audio.play_music("dungeon_finished")
      hero:set_direction(1)
      sahasrahla:set_position(544, 717)
      map:move_camera(544, 712, 100, function()
	map:set_dialog_variable("dungeon_7.sahasrahla", map:get_game():get_player_name())
	map:start_dialog("dungeon_7.sahasrahla", function()
	  hero:start_victory()
	end)
      end)
    end)
  end
end

function hero:on_victory_finished()
  map:get_game():set_dungeon_finished(7)
  hero:teleport(8, "from_dungeon_7")
end

function boss:on_dead()

  -- create the heart container manually to be sure it won't be in a hole
  map:create_pickable("heart_container", 1, 626, 544, 789, 0)
  if arrows_timer ~= nil then
    arrows_timer:stop()
    arrows_timer = nil
  end
end

