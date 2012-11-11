local map = ...
-- Dungeon 10 B1

local water_delay = 500 -- delay between each water step

local function set_water_drained()

  map:set_entities_enabled("water_tile", false)

  -- enable the custom obstacles near stairs (stairs can be used)
  map:set_entities_enabled("water_off_obstacle", true)

  -- disable the jumper placed over stairs (there is no water to jump into)
  map:set_entities_enabled("water_on_jumper", false)
end

local function drain_water_step_1()

  sol.audio.play_sound("water_drain_begin")
  sol.audio.play_sound("water_drain")
  water_tile_full:set_enabled(false)
  water_tile_less_1:set_enabled(true)
  sol.timer.start(water_delay, drain_water_step_2)
end

local function drain_water_step_2()

  water_tile_less_1:set_enabled(false)
  water_tile_less_2:set_enabled(true)
  sol.timer.start(water_delay, drain_water_step_3)
end

local function drain_water_step_3()

  water_tile_less_2:set_enabled(false)
  water_tile_less_3:set_enabled(true)
  sol.timer.start(water_delay, drain_water_step_4)
end

local function drain_water_step_4()

  water_tile_less_3:set_enabled(false)
  map:get_game():set_value("297", true)
  sol.audio.play_sound("secret")
  set_water_drained()
end

function map:on_started(destination)

  if map:get_game():get_value("234") then
    sensor1_1:set_enabled(false)
  end

  if map:get_game():get_value("297") then
    set_water_drained()
    water_block:set_position(656, 333)
  end

  map:set_entities_enabled("enemy2", false)
end

local function enemy_in_group2_dead(enemy)

  if not map:has_entities("enemy2")
      and not map:get_game():get_value("234") then
    map:create_pickable{
      treasure_name = "big_key",
      treasure_variant = 1,
      treasure_savegame_variable = 234,
      x = 672,
      y = 80,
      layer = 1
    }
    sol.audio.play_sound("secret")
    if not door_a:is_open() then
      map:open_doors("door_a")
      sol.audio.play_music("eagle_tower")
    end
  end
end
for _, enemy in ipairs(map:get_entities("enemy2")) do
  enemy.on_dead = enemy_in_group2_dead
end

function sensor1_1:on_activated()

  if door_a:is_open()
      and map:has_entities("enemy2")
      and not map:get_game():get_value("234") then
    sol.audio.play_music("boss")
    map:close_doors("door_a")
    map:set_entities_enabled("enemy2", true)
    self:set_enabled(false)
  end
end

function switch1_1:on_activated()
  map:open_doors("door_a")
end

function switch1_1:on_inactivated()
  map:close_doors("door_a")
end

function water_block:on_moved()
  map:move_camera(904, 288, 250, drain_water_step_1, 1000, 3500)
end

