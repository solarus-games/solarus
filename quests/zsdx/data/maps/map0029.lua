-- Twin caves script

current_switch_name = ""

function event_map_started(destination_point_name)
  
  if savegame_get_boolean(69) then
    tile_set_enabled("barrier", false)
    switch_set_enabled("barrier_switch", true)
  end
end

function event_switch_enabled(switch_name)
  
  current_switch_name = switch_name
  if switch_name == "barrier_switch" then
    camera_move(616, 672, 15)
  elseif switch_name == "door_switch" then
    camera_move(376, 384, 15)
  elseif switch_name == "final_barrier_switch" then
    tiles_set_enabled("final_barrier", false)
    play_sound("secret")
  end
end

function event_camera_reached_target()
  if current_switch_name == "door_switch" then
    timer_start(1000, "door_timer", false)
  elseif current_switch_name == "barrier_switch" then
    timer_start(1000, "barrier_timer", false)
  end
end

function barrier_timer()
  tile_set_enabled("barrier", false)
  play_sound("secret")
  savegame_set_boolean(69, true)
  timer_start(1000, "camera_restore", false)
end

function door_timer()
  door_open("door")
  play_sound("secret")
  timer_start(1000, "camera_restore", false)
end

