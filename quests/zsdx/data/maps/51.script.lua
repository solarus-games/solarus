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

local dont_close_LD06 = false

function map:on_started()

  map:set_light(0)
  if not map:get_game():get_value("725") then
    STT5:set_enabled(false)
    from_hole_tile:set_enabled(false)
  end
end

function DB03:on_activated()

  if DB04:is_activated()
      and not LD06:is_open() then
    map:open_doors("LD06")
  end
end

function DB04:on_activated()

  if DB03:is_activated()
      and not LD06:is_open() then
    map:open_doors("LD06")
  end
end

function DB03:on_inactivated()

  if not DB04:is_activated() then
    if LD06:is_open() and not dont_close_LD06 then
      map:close_doors("LD06")
    end
  end
end

function DB04:on_inactivated()

  if not DB03:is_activated() then
    if LD06:is_open() and not dont_close_LD06 then
      map:close_doors("LD06")
    end
  end
end

function dont_close_LD06_sensor:on_activated()
  dont_close_LD06 = true
end

