-- Dungeon 10 B1

local water_delay = 500 -- delay between each water step

function event_map_started(destination_point_name)

  if sol.map.get_game():get_boolean(234) then
    sol.map.sensor_set_enabled("sensor1_1",false)
  end

  if sol.map.get_game():get_boolean(297) then
    set_water_drained()
    sol.map.block_set_position("water_block", 656, 333)
  end

  sol.map.enemy_set_group_enabled("enemy2",false)
end

function event_enemy_dead(enemy_name)

  if enemy_name:find("enemy2")
      and sol.map.enemy_is_group_dead("enemy2")
      and not sol.map.get_game():get_boolean(234) then
    sol.map.pickable_item_create("big_key", 1, 234, 672, 80, 1)
    sol.audio.play_sound("secret")
    if not sol.map.door_is_open("door_a") then
      sol.map.door_open("door_a")
      sol.audio.play_music("eagle_tower")
    end
  end
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "sensor1_1"
      and sol.map.door_is_open("door_a")
      and not sol.map.enemy_is_group_dead("enemy2")
      and not sol.map.get_game():get_boolean(234) then
    sol.audio.play_music("boss")
    sol.map.door_close("door_a")
    sol.map.enemy_set_group_enabled("enemy2", true)
    sol.map.sensor_set_enabled("sensor1_1", false)
  end
end

function event_switch_activated(switch_name)

  if switch_name == "switch1_1" then
    sol.map.door_open("door_a")
  end
end

function event_switch_inactivated(switch_name)

  if switch_name == "switch1_1" then
    sol.map.door_close("door_a")
  end
end

function event_block_moved(block_name)

  if block_name == "water_block" then
    sol.map.camera_move(904, 288, 250, drain_water_step_1, 1000, 3500)
  end
end

function set_water_drained()

  sol.map.tile_set_group_enabled("water", false)

  -- enable the custom obstacles near stairs (stairs can be used)
  sol.map.obstacle_set_group_enabled("water_off_obstacle", true)

  -- disable the jumper placed over stairs (there is no water to jump into)
  sol.map.jumper_set_group_enabled("water_on_jumper", false)
end

function drain_water_step_1()

  sol.audio.play_sound("water_drain_begin")
  sol.audio.play_sound("water_drain")
  sol.map.tile_set_enabled("water_full", false)
  sol.map.tile_set_enabled("water_less_1", true)
  sol.timer.start(water_delay, drain_water_step_2)
end

function drain_water_step_2()

  sol.map.tile_set_enabled("water_less_1", false)
  sol.map.tile_set_enabled("water_less_2", true)
  sol.timer.start(water_delay, drain_water_step_3)
end

function drain_water_step_3()

  sol.map.tile_set_enabled("water_less_2", false)
  sol.map.tile_set_enabled("water_less_3", true)
  sol.timer.start(water_delay, drain_water_step_4)
end

function drain_water_step_4()

  sol.map.tile_set_enabled("water_less_3", false)
  sol.map.get_game():set_boolean(297, true)
  sol.audio.play_sound("secret")
  set_water_drained()
end

