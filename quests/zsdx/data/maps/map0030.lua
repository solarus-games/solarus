-------------------------
-- Dungeon 2 1F script --
-------------------------

function event_map_started(destination_point_name)

  if savegame_get_boolean(78) then
    tile_set_enabled("barrier", false)
    switch_set_enabled("barrier_switch", true);
  end
end

function event_switch_enabled(switch_name)

  if switch_name == "barrier_switch" then
    move_camera(120, 536, 15)
  end
end

function event_camera_reached_target()
  start_timer(1000, "barrier_camera_timer", false)
end

function barrier_camera_timer()
  play_sound("secret")
  tile_set_enabled("barrier", false)
  savegame_set_boolean(78, true)
  start_timer(1000, "restore_camera", false)
end

