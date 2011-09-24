-- Lyriann's cave B1

function event_switch_activated(switch_name)

  if switch_name == "open_barrier_1_switch" and sol.map.tile_is_enabled("barrier_1") then
    sol.map.tile_set_enabled("barrier_1", false)
    sol.main.play_sound("door_open")
    sol.map.switch_set_activated("close_barrier_1_switch", false)
  elseif switch_name == "close_barrier_1_switch" and not sol.map.tile_is_enabled("barrier_1") then
    sol.map.tile_set_enabled("barrier_1", true)
    sol.main.play_sound("door_closed")
    sol.map.switch_set_activated("open_barrier_1_switch", false)
  elseif switch_name == "open_barrier_2_switch" and sol.map.tile_is_enabled("barrier_2") then
    sol.map.tile_set_enabled("barrier_2", false)
    sol.main.play_sound("door_open")
    sol.map.switch_set_activated("close_barrier_2_switch", false)
  elseif switch_name == "close_barrier_2_switch" and not sol.map.tile_is_enabled("barrier_2") then
    sol.map.tile_set_enabled("barrier_2", true)
    sol.main.play_sound("door_closed")
    sol.map.switch_set_activated("open_barrier_2_switch", false)
  end
end

