local map = ...
-- Dungeon 7 3F

local fighting_boss = false
local just_removed_special_torch = false
local special_torch_timer = nil

local doors = { -- properties of the 5 timed doors
  door_a = { x = 920, y = 640, delay = 12000 },
  door_b = { x = 864, y = 808, delay = 15000 },
  door_c = { x = 1024, y = 840, delay = 12000 },
  door_d = { x = 832, y = 936, delay = 4000 },
  door_e = { x = 976, y = 952, delay = 4000 }
}
local current_door_name = nil -- current door during a camera movement
local door_timers = {} -- doors that currently have a timer running
local arrows_timer

function map:on_started(destination_point_name)

  -- block fallen into the hole
  if map:get_game():get_boolean(623) then
    map:block_set_enabled("nw_block", false)
  end

  -- NW door
  if map:get_game():get_boolean(624) then
    map:door_set_open("ne_door", true)
  end

  -- door A (timed doors)
  if map:get_game():get_boolean(627) then
    map:switch_set_activated("door_a_switch", true)
  end

  -- boss
  map:door_set_open("boss_door", true)
  if map:get_game():get_boolean(625)
    and not map:get_game():get_boolean(626) then
    -- boss killed, heart container not picked
    map:pickable_create("heart_container", 1, 626, 544, 789, 0)
  end

  -- special torch door
  if map:get_game():get_boolean(624) then
    map:switch_set_activated("ne_switch", true)
  end
end

function map:on_block_moved(block_name)

  if block_name == "nw_block" then
    local x, y = map:block_get_position(block_name)
    if x == 544 and y == 69 then
      -- make the block fall
      map:block_set_enabled(block_name, false)
      map:tile_set_enabled("hole_a", true)
      map:teletransporter_set_enabled("hole_a_teletransporter", true)
      map:get_game():set_boolean(623, true)
      sol.audio.play_sound("jump")
      sol.timer.start(500, function() sol.audio.play_sound("bomb") end)
    end
  end
end

function map:on_update()

  if not map:get_game():get_boolean(623) then
    -- blocks cannot overlap holes or teletransporters with the current engine,
    -- so we disable the hole A and its teletransporter when necessary

    local disable_hole = false
    local block_x, block_y = map:block_get_position("nw_block")
    if block_y == 69 and block_x >= 520 and block_x < 544 then
      -- the block is just before the hole
      local hero_x = map:hero_get_position()
      disable_hole = hero_x < block_x
    end

    if disable_hole and map:tile_is_enabled("hole_a") then
      map:tile_set_enabled("hole_a", false)
      map:teletransporter_set_enabled("hole_a_teletransporter", false)
    elseif not disable_hole and not map:tile_is_enabled("hole_a") then
      map:tile_set_enabled("hole_a", true)
      map:teletransporter_set_enabled("hole_a_teletransporter", true)
    end
  end
end

function map:on_switch_activated(switch_name)

  -- north-east room
  if switch_name == "ne_switch" then
    map:move_camera(960, 312, 250, function()
      sol.audio.play_sound("secret")
      map:door_open("ne_door")
    end)

  -- switch that removes the special torch
  elseif switch_name == "special_torch_switch" then
    map:move_camera(960, 120, 250, function()
      sol.audio.play_sound("secret")
      map:tile_set_enabled("special_torch", false)
      just_removed_special_torch = true
    end)

  -- timed doors
  else
    current_door_name = switch_name:match("^(door_[a-e])_switch$")
    if current_door_name ~= nil then
      door = doors[current_door_name]
      map:move_camera(door.x, door.y, 250, function()
	map:door_open(current_door_name)
      end)
    end
  end
end

function map:on_camera_back()

  -- set up a timer when the camera movement is finished
  if just_removed_special_torch then
    just_removed_special_torch = false
    special_torch_timer = sol.timer.start(8000, true, function()
      sol.audio.play_sound("door_closed")
      map:tile_set_enabled("special_torch", true)
      map:switch_set_activated("special_torch_switch", false)
      special_torch_timer = nil
    end)

  elseif current_door_name ~= nil then
    local door_name = current_door_name
    sol.timer.start(doors[door_name].delay, true, function()
      if door_timers[door_name] ~= nil then
	map:door_close(door_name)
	map:switch_set_activated(door_name .. "_switch", false)
	door_timers[door_name] = nil
      end
    end)
    door_timers[door_name] = true
    current_door_name = nil

  end
end

function map:on_hero_on_sensor(sensor_name)

  -- special torch
  if sensor_name == "special_torch_dont_close_sensor" then
    if special_torch_timer ~= nil then
      special_torch_timer:stop()
      special_torch_timer = nil
    end

  -- boss door
  elseif sensor_name == "close_boss_door_sensor"
      and map:door_is_open("boss_door") then
    map:door_close("boss_door")
    sol.audio.stop_music()

  -- boss
  elseif sensor_name == "start_boss_sensor"
      and not map:get_game():get_boolean(625)
      and not fighting_boss then
    
    start_boss()

  -- west room
  elseif sensor_name:find("w_room_sensor") then
    sol.audio.play_sound("secret")
    local state = map:tile_is_enabled("w_room_tile_1")
    map:tile_set_enabled("w_room_tile_1", not state)
    map:tile_set_enabled("w_room_tile_2", state)

  else
    -- pass a timed door
    local door_name = sensor_name:match("^(door_[a-e])_done_sensor$")
    if door_name ~= nil then
      door_timers[door_name] = nil -- disable the timer
    else

      -- close a timed door previously passed (i.e. it has no current timer)
      door_name = sensor_name:match("^(door_[a-e])_close_sensor$")
      if door_name ~= nil then
        if door_timers[door_name] == nil and map:door_is_open(door_name) then
	  map:door_close(door_name)
	  map:switch_set_activated(door_name .. "_switch", false)
	end
      end
    end
  end
end

function start_boss()

  sol.audio.play_music("boss")
  map:enemy_set_enabled("boss", true)
  fighting_boss = true

  arrows_timer = sol.timer.start(20000, repeat_give_arrows)
end

function repeat_give_arrows()

  -- give arrows if necessary during the boss fight
  if map:get_game():get_item_amount("bow") == 0 then
    local positions = {
      { x = 416, y = 685 },
      { x = 672, y = 685 },
      { x = 416, y = 885 },
      { x = 672, y = 885 },
    }
    arrow_xy = positions[math.random(#positions)]
    map:pickable_create("arrow", 3, -1, arrow_xy.x, arrow_xy.y, 0)
  end
  arrows_timer = sol.timer.start(20000, repeat_give_arrows)
end

function map:on_treasure_obtained(item_name, variant, savegame_variable)

  if item_name == "heart_container" then
    sol.audio.play_music("victory")
    map:hero_freeze()
    map:hero_set_direction(3)
    sol.timer.start(9000, start_final_sequence)
  end
end

function start_final_sequence()

  sol.audio.play_music("dungeon_finished")
  map:hero_set_direction(1)
  map:npc_set_position("sahasrahla", 544, 717)
  map:move_camera(544, 712, 100, function()
    map:start_dialog("dungeon_7.sahasrahla")
    map:set_dialog_variable("dungeon_7.sahasrahla", map:get_game():get_player_name());
  end)
end

function map:on_dialog_finished(dialog_id)

  if dialog_id == "dungeon_7.sahasrahla" then
    map:hero_start_victory()
  end
end

function map:on_hero_victory_finished()
  map:get_game():set_dungeon_finished(7)
  map:hero_set_map(8, "from_dungeon_7", 1)
end

function map:on_enemy_dead(enemy_name)

  if enemy_name == "boss" then
    -- create the heart container manually to be sure it won't be in a hole
    map:pickable_create("heart_container", 1, 626, 544, 789, 0)
    if arrows_timer ~= nil then
      arrows_timer:stop()
      arrows_timer = nil
    end
  end
end

