------------------------------
-- Lyriann's cave B1 script --
------------------------------

function event_switch_enabled(switch_name)

  if switch_name == "open_barrier_1_switch" and tile_is_enabled("barrier_1") then
    tile_set_enabled("barrier_1", false)
    play_sound("door_open")
    switch_set_enabled("close_barrier_1_switch", false)
  elseif switch_name == "close_barrier_1_switch" and not tile_is_enabled("barrier_1") then
    tile_set_enabled("barrier_1", true)
    play_sound("door_closed")
    switch_set_enabled("open_barrier_1_switch", false)
  elseif switch_name == "open_barrier_2_switch" and tile_is_enabled("barrier_2") then
    tile_set_enabled("barrier_2", false)
    play_sound("door_open")
    switch_set_enabled("close_barrier_2_switch", false)
  elseif switch_name == "close_barrier_2_switch" and not tile_is_enabled("barrier_2") then
    tile_set_enabled("barrier_2", true)
    play_sound("door_closed")
    switch_set_enabled("open_barrier_2_switch", false)
  end


end

