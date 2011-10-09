-- Inferno river maze B1

water_delay = 500 -- delay between each water step
current_pool_index = 0
camera_timer = nil

pools = {
  { initially_filled = false, trigger = "switch", x = 672, y = 96  },
  { initially_filled = true,  trigger = "block",  x = 800, y = 272 },
  { initially_filled = false, trigger = "switch", x = 560, y = 336 },
  { initially_filled = true,  trigger = "block",  x = 96,  y = 96  },
  { initially_filled = false, trigger = "switch", x = 336, y = 544 },
  { initially_filled = false, trigger = "switch", x = 96,  y = 338 },
  { initially_filled = false, trigger = "switch", x = 416, y = 96  },
}
savegame_variable = 170

function event_map_started(destination_point_name)

  -- initialize all pools
  for i, pool in ipairs(pools) do
    if pool.initially_filled ~= sol.game.savegame_get_boolean(savegame_variable + i) then
      -- this pool is filled
      set_water_filled(i)
    else
      -- this pool is not filled
      set_water_drained(i)
    end
  end
end

function event_switch_activated(switch_name)

  local matched = string.match(switch_name, "^water_([1-9])_on_switch$")
  if matched then
    local index = tonumber(matched)
    fill_water(index)
  end
end

function event_block_moved(block_name)

  local matched = string.match(block_name, "^water_([1-9])_on_block$")
  if matched then
    local index = tonumber(matched)
    fill_water(index)
  else
    matched = string.match(block_name, "^water_([1-9])_off_block$")
    if matched then
      local index = tonumber(matched)
      drain_water(index)
    end
  end
end

function fill_water(index)
  current_pool_index = index
  camera_timer = fill_water_step_1
  sol.map.camera_move(pools[index].x, pools[index].y, 250)
end

function drain_water(index)
  current_pool_index = index
  camera_timer = drain_water_step_1
  sol.map.camera_move(pools[index].x, pools[index].y, 250)
end

function set_water_filled(i)

  -- show the water tile
  sol.map.tile_set_enabled("water_"..i.."_full", true)

  -- disable the custom obstacles near stairs (stairs are useless)
  sol.map.obstacle_set_group_enabled("water_"..i.."_off_obstacle", false)

  -- enable the jump sensors placed over stairs (they are used to jump into water)
  sol.map.jump_sensor_set_group_enabled("water_"..i.."_on_jump_sensor", true)

  if pools[i].trigger == "switch" then
    -- make activated the switch that fills this pool
    sol.map.switch_set_activated("water_"..i.."_on_switch", true)
  else
    -- hide the block that fills this pool
    sol.map.block_set_enabled("water_"..i.."_on_block", false)
    -- reset and show the block that drains this pool
    sol.map.block_reset("water_"..i.."_off_block")
    sol.map.block_set_enabled("water_"..i.."_off_block", true)
  end
end

function set_water_drained(i)

  -- enable the custom obstacles near stairs (stairs can be used)
  sol.map.obstacle_set_group_enabled("water_"..i.."_off_obstacle", true)

  -- disable the jump sensors placed over stairs (there is no water to jump into)
  sol.map.jump_sensor_set_group_enabled("water_"..i.."_on_jump_sensor", false)

  if pools[i].trigger == "block" then
    -- hide the block that drains this pool
    sol.map.block_set_enabled("water_"..i.."_off_block", false)
    -- reset and show the block that fills this pool
    sol.map.block_reset("water_"..i.."_on_block")
    sol.map.block_set_enabled("water_"..i.."_on_block", true)
  end
end

function event_camera_reached_target()
  sol.main.timer_start(camera_timer, 1000)
end

function fill_water_step_1()
  sol.main.play_sound("water_fill_begin")
  sol.main.play_sound("water_fill")
  sol.map.tile_set_enabled("water_"..current_pool_index.."_less_3", true)
  sol.main.timer_start(fill_water_step_2, water_delay)
end

function fill_water_step_2()
  sol.map.tile_set_enabled("water_"..current_pool_index.."_less_3", false)
  sol.map.tile_set_enabled("water_"..current_pool_index.."_less_2", true)
  sol.main.timer_start(fill_water_step_3, water_delay)
end

function fill_water_step_3()
  sol.map.tile_set_enabled("water_"..current_pool_index.."_less_2", false)
  sol.map.tile_set_enabled("water_"..current_pool_index.."_less_1", true)
  sol.main.timer_start(fill_water_step_4, water_delay)
end

function fill_water_step_4()
  sol.map.tile_set_enabled("water_"..current_pool_index.."_less_1", false)
  sol.map.tile_set_enabled("water_"..current_pool_index.."_full", true)
  sol.main.timer_start(sol.map.camera_restore, 500)
  sol.game.savegame_set_boolean(savegame_variable + current_pool_index,
    not pools[current_pool_index].initially_filled)
  set_water_filled(current_pool_index)
end

function drain_water_step_1()
  sol.main.play_sound("water_drain_begin")
  sol.main.play_sound("water_drain")
  sol.map.tile_set_enabled("water_"..current_pool_index.."_full", false)
  sol.map.tile_set_enabled("water_"..current_pool_index.."_less_1", true)
  sol.main.timer_start(drain_water_step_2, water_delay)
end

function drain_water_step_2()
  sol.map.tile_set_enabled("water_"..current_pool_index.."_less_1", false)
  sol.map.tile_set_enabled("water_"..current_pool_index.."_less_2", true)
  sol.main.timer_start(drain_water_step_3, water_delay)
end

function drain_water_step_3()
  sol.map.tile_set_enabled("water_"..current_pool_index.."_less_2", false)
  sol.map.tile_set_enabled("water_"..current_pool_index.."_less_3", true)
  sol.main.timer_start(drain_water_step_4, water_delay)
end

function drain_water_step_4()
  sol.map.tile_set_enabled("water_"..current_pool_index.."_less_3", false)
  sol.main.timer_start(sol.map.camera_restore, 500)
  sol.game.savegame_set_boolean(savegame_variable + current_pool_index,
    pools[current_pool_index].initially_filled)
  set_water_drained(current_pool_index)
end


