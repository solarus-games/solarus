-------------------------
-- Lyriann cave script --
-------------------------

function event_map_started()
  if savegame_get_boolean(38) then
    disable_tile("barrier")
    switch_set_enabled("open_barrier_switch", true)
  end
end

function event_switch_enabled(switch_name)
  move_camera(136, 304, 10)
end

function event_camera_reached_target()
  start_timer(1000, "camera_timer", false)
end

function camera_timer()
  play_sound("secret")
  disable_tile("barrier")
  savegame_set_boolean(38, true)
  start_timer(1000, "restore_camera", false)
end
