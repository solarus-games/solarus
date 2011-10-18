function event_map_started(destination_point_name)
if sol.game.savegame_get_boolean(235) then
	sol.map.sensor_set_enabled("sensor1_1",false)
end
 sol.map.enemy_set_group_enabled("enemy2",false)
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


function event_enemy_dead(enemy_name)

	if sol.map.enemy_is_group_dead("enemy2") and not sol.game.savegame_get_boolean(235) then
		sol.map.pickable_item_create("big_key", 1, "234", 672,80, 1)
		sol.main.play_sound("secret")
		sol.main.play_sound("door_open")
		sol.map.door_open("door1")
		sol.game.savegame_set_boolean(235, true)
		sol.main.play_music("eagle_tower.it")
	end

end

function event_hero_on_sensor(sensor_name)

	if sensor_name=="sensor1_1" then
		sol.main.play_music("boss.spc")
		sol.main.play_sound("door_closed")
		sol.map.door_close("door1")
		sol.map.enemy_set_group_enabled("enemy2",true)
		sol.map.sensor_set_enabled("sensor1_1",false)
	end
end
function event_switch_activated(switch_name)


  local matched = string.match(switch_name, "^water_([1-9])_on_switch$")
  if matched then
    local index = tonumber(matched)
    fill_water(index)
  end

	if switch_name=="switch1_1" then
		sol.main.play_sound("door_open")
		sol.map.door_open("door1")
	end

end

function event_switch_inactivated(switch_name)

	if switch_name=="switch1_1" then
		sol.main.play_sound("door_closed")
		sol.map.door_close("door1")
	end

end



water_delay = 500 -- delay between each water step
current_pool_index = 0

pools = {
  { initially_filled = false, trigger = "switch", x = 672, y = 96  },
  { initially_filled = true,  trigger = "block",  x = 800, y = 272 }
}
savegame_variable = 270



function event_block_moved(block_name)

  local matched = string.match(block_name, "^water_([1-9])_on_block$")

    if matched then
      local index = tonumber(matched)
      drain_water(index)
end
end

function fill_water(index)
  current_pool_index = index
  sol.map.camera_move(pools[index].x, pools[index].y, 250, fill_water_step_1, 1000, 2500)
end

function drain_water(index)
  current_pool_index = index
  sol.map.camera_move(pools[index].x, pools[index].y, 250, drain_water_step_1, 1000, 2500)
end

function set_water_filled(i)

  -- show the water tile
  sol.map.tile_set_enabled("water_"..i.."_full", true)

  -- disable the custom obstacles near stairs (stairs are useless)
  sol.map.obstacle_set_group_enabled("water_"..i.."_off_obstacle", false)

  -- enable the jumpers placed over stairs (they are used to jump into water)
  sol.map.jumper_set_group_enabled("water_"..i.."_on_jumper", true)

  if pools[i].trigger == "switch" then
    -- make activated the switch that fills this pool
    sol.map.switch_set_activated("water_"..i.."_on_switch", true)
  end
end

function set_water_drained(i)

  -- enable the custom obstacles near stairs (stairs can be used)
  sol.map.obstacle_set_group_enabled("water_"..i.."_off_obstacle", true)

  -- disable the jumpers placed over stairs (there is no water to jump into)
  sol.map.jumper_set_group_enabled("water_"..i.."_on_jumper", false)

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
  sol.game.savegame_set_boolean(savegame_variable + current_pool_index,
    pools[current_pool_index].initially_filled)
  set_water_drained(current_pool_index)
end


