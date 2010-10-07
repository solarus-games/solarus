-- Twin caves script

current_switch_name = ""

function event_map_started(destination_point_name)
  
  if sol.game.savegame_get_boolean(69) then
    sol.map.tile_set_enabled("barrier", false)
    sol.map.switch_set_enabled("barrier_switch", true)
  end
end

function event_switch_enabled(switch_name)
  
  current_switch_name = switch_name
  if switch_name == "barrier_switch" then
    sol.map.camera_move(616, 672, 15)
  elseif switch_name == "door_switch" then
    sol.map.camera_move(376, 384, 15)
  elseif switch_name == "final_barrier_switch" then
    sol.map.tile_set_group_enabled("final_barrier", false)
    sol.main.play_sound("secret")
  end
end

function event_camera_reached_target()
  if current_switch_name == "door_switch" then
    sol.main.timer_start(1000, "door_timer", false)
  elseif current_switch_name == "barrier_switch" then
    sol.main.timer_start(1000, "barrier_timer", false)
  end
end

function barrier_timer()
  sol.map.tile_set_enabled("barrier", false)
  sol.main.play_sound("secret")
  sol.game.savegame_set_boolean(69, true)
  sol.main.timer_start(1000, "sol.map.camera_restore", false)
end

function door_timer()
  sol.map.door_open("door")
  sol.main.play_sound("secret")
  sol.main.timer_start(1000, "sol.map.camera_restore", false)
end

