-- Twin caves script

function event_map_started(destination_point_name)
  
  if savegame_get_boolean(69) then
    tile_set_enabled("barrier", false)
  end
end

function event_switch_enabled(switch_name)
  
  if switch_name == "barrier_switch" then
    move_camera(616, 672, 15)
  end
end

function event_camera_reached_target()
  start_timer(1000, "barrier_timer", false)
end

function barrier_timer()
  tile_set_enabled("barrier", false)
  play_sound("secret")
  savegame_set_boolean(69, true)
  start_timer(1000, "restore_camera", false)
end

