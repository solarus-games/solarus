local map = ...
-- Inferno river maze B1

local water_delay = 500 -- delay between each water step
local current_pool_index = 0

local pools = {
  { initially_filled = false, trigger = "switch", x = 672, y = 96  },
  { initially_filled = true,  trigger = "block",  x = 800, y = 272 },
  { initially_filled = false, trigger = "switch", x = 560, y = 336 },
  { initially_filled = true,  trigger = "block",  x = 96,  y = 96  },
  { initially_filled = false, trigger = "switch", x = 336, y = 544 },
  { initially_filled = false, trigger = "switch", x = 96,  y = 338 },
  { initially_filled = false, trigger = "switch", x = 416, y = 96  },
}
local savegame_variable = 170

function map:on_started(destination_point)

  -- initialize all pools
  for i, pool in ipairs(pools) do
    if pool.initially_filled ~= map:get_game():get_boolean(savegame_variable + i) then
      -- this pool is filled
      set_water_filled(i)
    else
      -- this pool is not filled
      set_water_drained(i)
    end
  end
end

for i in 1, 9 do
  local switch = map:get_entity("water_" .. i "_on_switch")
  if switch ~= nil then
    switch:on_activated = water_switch_activated
  end
end

function water_switch_ativated(switch)

  local matched = switch:get_name():match("^water_([1-9])_on_switch$")
  if matched then
    local index = tonumber(matched)
    fill_water(index)
  end
end

for i in 1, 9 do
  local block = map:get_entity("water_" .. i "_on_block")
  if block ~= nil then
    block:on_moved = water_block_moved
  end
  block = map:get_entity("water_" .. i "_off_block")
  if block ~= nil then
    block:on_moved = water_block_moved
  end
end

function water_block_moved(block)

  local matched = block:get_name():match("^water_([1-9])_on_block$")
  if matched then
    local index = tonumber(matched)
    fill_water(index)
  else
    matched = block:get_name():match("^water_([1-9])_off_block$")
    if matched then
      local index = tonumber(matched)
      drain_water(index)
    end
  end
end

function fill_water(index)
  current_pool_index = index
  map:move_camera(pools[index].x, pools[index].y, 250, fill_water_step_1, 1000, 2500)
end

function drain_water(index)
  current_pool_index = index
  map:move_camera(pools[index].x, pools[index].y, 250, drain_water_step_1, 1000, 2500)
end

function set_water_filled(i)

  -- show the water tile
  map:get_entity("water_" .. i .. "_full"):set_enabled(true)

  -- disable the walls near stairs (stairs are useless)
  map:set_entities_enabled("water_" .. i .. "_off_obstacle", false)

  -- enable the jumpers placed over stairs (they are used to jump into water)
  map:set_entities_enabled("water_" .. i .. "_on_jumper", true)

  if pools[i].trigger == "switch" then
    -- make activated the switch that fills this pool
    map:get_entity("water_" .. i .. "_on_switch"):set_activated(true)
  else
    -- hide the block that fills this pool
    map:get_entity("water_" .. i .. "_on_block"):set_enabled(false)
    -- reset and show the block that drains this pool
    map:get_entity("water_" .. i .. "_off_block"):reset()
    map:get_entity("water_" .. i .. "_off_block"):set_enabled(true)
  end
end

function set_water_drained(i)

  -- enable the custom obstacles near stairs (stairs can be used)
  map:set_entities_enabled("water_" .. i .. "_off_obstacle", true)

  -- disable the jumpers placed over stairs (there is no water to jump into)
  map:set_entities_enabled("water_" .. i .. "_on_jumper", false)

  if pools[i].trigger == "block" then
    -- hide the block that drains this pool
    map:get_entity("water_" .. i .. "_off_block"):set_enabled(false)
    -- reset and show the block that fills this pool
    map:get_entity("water_" .. i .. "_on_block"):reset()
    map:get_entity("water_" .. i .. "_on_block"):set_enabled(true)
  end
end

function fill_water_step_1()
  sol.audio.play_sound("water_fill_begin")
  sol.audio.play_sound("water_fill")
  map:get_entity("water_" .. current_pool_index .. "_less_3"):set_enabled(true)
  sol.timer.start(water_delay, fill_water_step_2)
end

function fill_water_step_2()
  map:get_entity("water_" .. current_pool_index .. "_less_3"):set_enabled(false)
  map:get_entity("water_" .. current_pool_index .. "_less_2"):set_enabled(true)
  sol.timer.start(water_delay, fill_water_step_3)
end

function fill_water_step_3()
  map:get_entity("water_" .. current_pool_index .. "_less_2"):set_enabled(false)
  map:get_entity("water_" .. current_pool_index .. "_less_1"):set_enabled(true)
  sol.timer.start(water_delay, fill_water_step_4)
end

function fill_water_step_4()
  map:get_entity("water_" .. current_pool_index .. "_less_1"):set_enabled(false)
  map:get_entity("water_" .. current_pool_index .. "_full"):set_enabled(true)
  map:get_game():set_boolean(savegame_variable + current_pool_index,
    not pools[current_pool_index].initially_filled)
  set_water_filled(current_pool_index)
end

function drain_water_step_1()
  sol.audio.play_sound("water_drain_begin")
  sol.audio.play_sound("water_drain")
  map:get_entity("water_" .. current_pool_index .. "_full"):set_enabled(false)
  map:get_entity("water_" .. current_pool_index .. "_less_1"):set_enabled(true)
  sol.timer.start(water_delay, drain_water_step_2)
end

function drain_water_step_2()
  map:get_entity("water_" .. current_pool_index .. "_less_1"):set_enabled(false)
  map:get_entity("water_" .. current_pool_index .. "_less_2"):set_enabled(true)
  sol.timer.start(water_delay, drain_water_step_3)
end

function drain_water_step_3()
  map:get_entity("water_" .. current_pool_index .. "_less_2"):set_enabled(false)
  map:get_entity("water_" .. current_pool_index .. "_less_3"):set_enabled(true)
  sol.timer.start(water_delay, drain_water_step_4)
end

function drain_water_step_4()
  map:get_entity("water_" .. current_pool_index .. "_less_3"):set_enabled(false)
  map:get_game():set_boolean(savegame_variable + current_pool_index,
    pools[current_pool_index].initially_filled)
  set_water_drained(current_pool_index)
end

