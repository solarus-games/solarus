-- Dungeon 7 3F

just_removed_special_torch = false
special_torch_allow_close = true

function event_map_started(destination_point_name)

  -- block fallen into the hole
  if sol.game.savegame_get_boolean(623) then
    sol.map.block_set_enabled("nw_block", false)
  end

  -- NW door
  if sol.game.savegame_get_boolean(624) then
    sol.map.door_set_open("ne_door", true)
  end
end

function event_block_moved(block_name)

  if block_name == "nw_block" then
    local x, y = sol.map.block_get_position(block_name)
    if x == 544 and y == 69 then
      -- make the block fall
      sol.map.block_set_enabled(block_name, false)
      sol.map.tile_set_enabled("hole_a", true)
      sol.map.teletransporter_set_enabled("hole_a_teletransporter", true)
      sol.game.savegame_set_boolean(623, true)
      sol.main.play_sound("jump")
      sol.main.timer_start(function() sol.main.play_sound("bomb") end, 500)
    end
  end
end

function event_update()

  if not sol.game.savegame_get_boolean(623) then
    -- blocks cannot overlap holes or teletransporters with the current engine,
    -- so we disable the hole A and its teletransporter when necessary

    local disable_hole = false
    local block_x, block_y = sol.map.block_get_position("nw_block")
    if block_y == 69 and block_x >= 520 and block_x < 544 then
      -- the block is just before the hole
      local hero_x = sol.map.hero_get_position()
      disable_hole = hero_x < block_x
    end

    if disable_hole and sol.map.tile_is_enabled("hole_a") then
      sol.map.tile_set_enabled("hole_a", false)
      sol.map.teletransporter_set_enabled("hole_a_teletransporter", false)
    elseif not disable_hole and not sol.map.tile_is_enabled("hole_a") then
      sol.map.tile_set_enabled("hole_a", true)
      sol.map.teletransporter_set_enabled("hole_a_teletransporter", true)
    end
  end
end

function event_switch_activated(switch_name)

  -- north-east room
  if switch_name == "ne_switch" then
    sol.map.camera_move(960, 312, 250, function()
      sol.main.play_sound("secret")
      sol.map.door_open("ne_door")
    end)

  -- switch that removes the special torch
  elseif switch_name == "special_torch_switch" then
    sol.map.camera_move(960, 120, 250, function()
      sol.main.play_sound("secret")
      sol.map.tile_set_enabled("special_torch", false)
      just_removed_special_torch = true
    end)
  end
end

function event_camera_back()

  if just_removed_special_torch then
    -- set up a timer when the camera movement is finished
    just_removed_special_torch = false
    sol.main.timer_start(function()
      sol.map.switch_set_activated("special_torch_switch", false)
      if special_torch_allow_close then
        sol.main.play_sound("door_closed")
        sol.map.tile_set_enabled("special_torch", true)
      end
    end, 8000, true)
  end
end

function event_hero_on_sensor(sensor_name)

  -- special torch
  if sensor_name == "special_torch_dont_close_sensor" then
    special_torch_allow_close = false
  elseif sensor_name == "special_torch_close_sensor" then
    special_torch_allow_close = true
    if not sol.map.switch_is_activated("special_torch_switch") then
      -- the timer has expired in the meantime
      sol.main.play_sound("door_closed")
      sol.map.tile_set_enabled("special_torch", true)
    end

  -- west room
  elseif sensor_name:find("w_room_sensor") then
    sol.main.play_sound("secret")
    local state = sol.map.tile_is_enabled("w_room_tile_1")
    sol.map.tile_set_enabled("w_room_tile_1", not state)
    sol.map.tile_set_enabled("w_room_tile_2", state)
  end
end

