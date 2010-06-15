-------------------------
-- Dungeon 2 1F script --
-------------------------

function event_map_started(destination_point_name)

  if savegame_get_boolean(78) then
    tile_set_enabled("barrier", false)
    switch_set_enabled("barrier_switch", true)
  end

  if savegame_get_boolean(90) then
    open_hidden_stairs()
  end

  if savegame_get_boolean(91) then
    open_hidden_door()
  end
end

function event_switch_enabled(switch_name)

  if switch_name == "barrier_switch" then
    move_camera(120, 536, 15)
  elseif switch_name == "tmp_switch" then
    open_hidden_stairs()
    open_hidden_door()
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

function open_hidden_stairs()
  tiles_set_enabled("hidden_stairs_closed", false)
  tiles_set_enabled("hidden_stairs_open", true)
end

function open_hidden_door()
  tiles_set_enabled("hidden_door_closed", false)
  tiles_set_enabled("hidden_door_open", true)
end

