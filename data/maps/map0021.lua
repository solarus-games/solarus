------------------------------
-- Lyriann's cave B1 script --
------------------------------

function event_switch_enabled(switch_name)

  if switch_name == "open_barrier_1_switch" and is_tile_enabled("barrier_1") then
    disable_tile("barrier_1")
    play_sound("door_open")
    switch_set_enabled("close_barrier_1_switch", false)
  elseif switch_name == "close_barrier_1_switch" and not is_tile_enabled("barrier_1") then
    enable_tile("barrier_1")
    play_sound("door_closed")
    switch_set_enabled("open_barrier_1_switch", false)
  elseif switch_name == "open_barrier_2_switch" and is_tile_enabled("barrier_2") then
    disable_tile("barrier_2")
    play_sound("door_open")
    switch_set_enabled("close_barrier_2_switch", false)
  elseif switch_name == "close_barrier_2_switch" and not is_tile_enabled("barrier_2") then
    enable_tile("barrier_2")
    play_sound("door_closed")
    switch_set_enabled("open_barrier_2_switch", false)
  end


end

