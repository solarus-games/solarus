local map = ...
-- Dungeon 8 B4

-- Legend
-- CB: Chest Button
-- RC: Rupee Chest
-- KC: Key Chest
-- KP: Key Pot
-- LD: Locked Door
-- KD: Key Door
-- DB: Door Button
-- LB: Locked Barrier
-- BB: Barrier Button
-- DS: Door Sensor

dont_close_LD06 = false

function map:on_started()

  map:set_light(0)
  if not map:get_game():get_boolean(725) then
    map:block_set_enabled("STT5", false)
    map:tile_set_enabled("from_hole_tile", false)
  end
end

function map:on_switch_activated(switch_name)

  if map:switch_is_activated("DB03")
      and map:switch_is_activated("DB04")
      and not map:door_is_open("LD06") then
    map:door_open("LD06")
  end
end

function map:on_switch_inactivated(switch_name)

  if not map:switch_is_activated("DB03")
      or not map:switch_is_activated("DB04") then
    if map:door_is_open("LD06") and not dont_close_LD06 then
      map:door_close("LD06")
    end
  end
end

function map:on_hero_on_sensor(sensor_name)

  if sensor_name == "dont_close_LD06_sensor" then
    dont_close_LD06 = true
  end
end

