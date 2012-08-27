local map = ...
-- Dungeon 10 B1

local water_delay = 500 -- delay between each water step

function map:on_started(destination_point)

  if map:get_game():get_boolean(234) then
    sensor1_1:set_enabled(false)
  end

  if map:get_game():get_boolean(297) then
    set_water_drained()
    map:block_set_position("water_block", 656, 333)
  end

  map:set_entities_enabled("enemy2",false)
end

function map:on_enemy_dead(enemy_name)

  if enemy_name:find("enemy2")
      and not map:has_entities("enemy2")
      and not map:get_game():get_boolean(234) then
    map:create_pickable("big_key", 1, 234, 672, 80, 1)
    sol.audio.play_sound("secret")
    if not door_a:is_open() then
      map:open_doors("door_a")
      sol.audio.play_music("eagle_tower")
    end
  end
end

function map:on_hero_on_sensor(sensor_name)

  if sensor_name == "sensor1_1"
      and door_a:is_open()
      and map:has_entities("enemy2")
      and not map:get_game():get_boolean(234) then
    sol.audio.play_music("boss")
    map:close_doors("door_a")
    map:set_entities_enabled("enemy2", true)
    sensor1_1:set_enabled(false)
  end
end

function map:on_switch_activated(switch_name)

  if switch_name == "switch1_1" then
    map:open_doors("door_a")
  end
end

function map:on_switch_inactivated(switch_name)

  if switch_name == "switch1_1" then
    map:close_doors("door_a")
  end
end

function map:on_block_moved(block_name)

  if block_name == "water_block" then
    map:move_camera(904, 288, 250, drain_water_step_1, 1000, 3500)
  end
end

function set_water_drained()

  map:set_entities_enabled("water", false)

  -- enable the custom obstacles near stairs (stairs can be used)
  map:set_entities_enabled("water_off_obstacle", true)

  -- disable the jumper placed over stairs (there is no water to jump into)
  map:set_entities_enabled("water_on_jumper", false)
end

function drain_water_step_1()

  sol.audio.play_sound("water_drain_begin")
  sol.audio.play_sound("water_drain")
  water_full:set_enabled(false)
  water_less_1:set_enabled(true)
  sol.timer.start(water_delay, drain_water_step_2)
end

function drain_water_step_2()

  water_less_1:set_enabled(false)
  water_less_2:set_enabled(true)
  sol.timer.start(water_delay, drain_water_step_3)
end

function drain_water_step_3()

  water_less_2:set_enabled(false)
  water_less_3:set_enabled(true)
  sol.timer.start(water_delay, drain_water_step_4)
end

function drain_water_step_4()

  water_less_3:set_enabled(false)
  map:get_game():set_boolean(297, true)
  sol.audio.play_sound("secret")
  set_water_drained()
end

