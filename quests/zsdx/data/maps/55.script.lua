local map = ...
-- Dungeon 6 2F

local remove_water_delay = 500

local function remove_water()
  sol.audio.play_sound("water_drain_begin")
  sol.audio.play_sound("water_drain")
  water_tile_less_1:set_enabled(true)
  water_tile_full:set_enabled(false)
  sol.timer.start(remove_water_delay, remove_water_2)
end

local function remove_water_2()
  water_tile_less_2:set_enabled(true)
  water_tile_less_1:set_enabled(false)
  sol.timer.start(remove_water_delay, remove_water_3)
end

local function remove_water_3()
  water_tile_less_3:set_enabled(true)
  water_tile_less_2:set_enabled(false)
  sol.timer.start(remove_water_delay, remove_water_4)
end

local function remove_water_4()
  water_tile_less_3:set_enabled(false)
  map:set_entities_enabled("water_on_jumper", false)
  map:set_entities_enabled("water_off_obstacle", true)
  map:get_game():set_value("319", true)
  sol.audio.play_sound("secret")
end

function map:on_started(destination)

  -- weak floor
  if map:get_game():get_value("314") then
    weak_floor:set_enabled(false)
    weak_floor_sensor:set_enabled(false)
  else
    weak_floor_teletransporter:set_enabled(false)
  end

  -- water
  if map:get_game():get_value("319") then
    map:set_entities_enabled("water_tile", false)
    map:set_entities_enabled("water_on_jumper", false)
    water_switch:set_activated(true)
  else
    map:set_entities_enabled("water_off_obstacle", false)
  end
end

function above_barrier_sensor:on_activated()

  -- fix the position of the hero when he lands on barriers after the feather jump
  local x, y = hero:get_position()
  if y > 629 and y < 641 then
    hero:set_position(x, 629, 0)
  elseif y >= 641 and y < 653 then
    hero:set_position(x, 653, 0)
  end
end

function weak_floor_sensor:on_collision_explosion()

  if weak_floor:is_enabled() then
    weak_floor:set_enabled(false)
    weak_floor_sensor:set_enabled(false)
    weak_floor_teletransporter:set_enabled(true)
    sol.audio.play_sound("secret")
    map:get_game():set_value("314", true)
  end
end

function water_switch:on_activated()

  if not map:get_game():get_value("319") then
    map:move_camera(616, 192, 250, remove_water, 1000, 2500)
  end
end

