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

function event_map_started()

  sol.map.light_set(0)
  if not sol.game.savegame_get_boolean(725) then
    sol.map.block_set_enabled("STT5", false)
    sol.map.tile_set_enabled("from_hole_tile", false)
  end
end

function event_switch_activated(switch_name)

  if sol.map.switch_is_activated("DB03")
      and sol.map.switch_is_activated("DB04") then
    sol.map.door_open("LD06")
  end
end

function event_switch_inactivated(switch_name)

  if not sol.map.switch_is_activated("DB03")
      or not sol.map.switch_is_activated("DB04") then
    sol.map.door_close("LD06")
  end
end

