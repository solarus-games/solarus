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

DB03_status = false
DB04_status = false

function event_map_started()
  sol.map.light_set(0)
  if not sol.game.savegame_get_boolean(725) then
    sol.map.block_set_enabled("STT5", false)
    sol.map.tile_set_enabled("from_hole_tile", false)
  end
end

function event_switch_activated(switch_name)
  if switch_name == "DB03" then
    DB03_status = true
  elseif switch_name == "DB04" then
    DB04_status = true
  end
  if DB03_status == true and DB04_status == true then
    if not sol.map.door_is_open("LD06") then
      sol.map.door_open("LD06")
    end
  end
end

function event_switch_inactivated(switch_name)
  if switch_name == "DB03" then
    DB03_status = false
  elseif switch_name == "DB04" then
    DB04_status = false
  end
  if DB03_status == false or DB04_status == false then
    if sol.map.door_is_open("LD06") then
      sol.map.door_close("LD06")
    end
  end
end
