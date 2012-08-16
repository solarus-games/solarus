local map = ...
-- Lyriann's cave B1

function map:on_switch_activated(switch_name)

  if switch_name == "open_barrier_1_switch" and map:tile_is_enabled("barrier_1") then
    map:tile_set_enabled("barrier_1", false)
    sol.audio.play_sound("door_open")
    map:switch_set_activated("close_barrier_1_switch", false)
  elseif switch_name == "close_barrier_1_switch" and not map:tile_is_enabled("barrier_1") then
    map:tile_set_enabled("barrier_1", true)
    sol.audio.play_sound("door_closed")
    map:switch_set_activated("open_barrier_1_switch", false)
  elseif switch_name == "open_barrier_2_switch" and map:tile_is_enabled("barrier_2") then
    map:tile_set_enabled("barrier_2", false)
    sol.audio.play_sound("door_open")
    map:switch_set_activated("close_barrier_2_switch", false)
  elseif switch_name == "close_barrier_2_switch" and not map:tile_is_enabled("barrier_2") then
    map:tile_set_enabled("barrier_2", true)
    sol.audio.play_sound("door_closed")
    map:switch_set_activated("open_barrier_2_switch", false)
  end
end

