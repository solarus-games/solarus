-- Twin caves

function event_map_started(destination_point_name)
  
  if sol.game.savegame_get_boolean(69) then
    sol.map.tile_set_enabled("barrier", false)
    sol.map.switch_set_activated("barrier_switch", true)
  end
end

function event_switch_activated(switch_name)
  
  current_switch_name = switch_name
  if switch_name == "barrier_switch" then
    sol.map.camera_move(616, 672, 250, barrier_timer)
  elseif switch_name == "door_switch" then
    sol.map.camera_move(376, 384, 250, door_timer)
  elseif switch_name == "final_barrier_switch" then
    sol.map.tile_set_group_enabled("final_barrier", false)
    sol.main.play_sound("secret")
  end
end

function barrier_timer()
  sol.map.tile_set_enabled("barrier", false)
  sol.main.play_sound("secret")
  sol.game.savegame_set_boolean(69, true)
end

function door_timer()
  sol.map.door_open("door")
  sol.main.play_sound("secret")
end

function event_door_open(door_name)

  if door_name == "weak_block" then
    sol.main.play_sound("secret")
  end
end

