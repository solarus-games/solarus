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
    STT5:set_enabled(false)
    from_hole_tile:set_enabled(false)
  end
end

function map:on_switch_activated(switch_name)

  if DB03:is_activated()
      and DB04:is_activated()
      and not LD06:is_open() then
    map:open_doors("LD06")
  end
end

function map:on_switch_inactivated(switch_name)

  if not DB03:is_activated()
      or not DB04:is_activated() then
    if LD06:is_open() and not dont_close_LD06 then
      map:close_doors("LD06")
    end
  end
end

function map:on_hero_on_sensor(sensor_name)

  if sensor_name == "dont_close_LD06_sensor" then
    dont_close_LD06 = true
  end
end

