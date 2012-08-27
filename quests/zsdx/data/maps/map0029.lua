local map = ...
-- Twin caves

function map:on_started(destination_point)

  if map:get_game():get_boolean(69) then
    barrier:set_enabled(false)
    barrier_switch:set_activated(true)
  end
end

function map:on_switch_activated(switch_name)

  current_switch_name = switch_name
  if switch_name == "barrier_switch" then
    map:move_camera(616, 672, 250, barrier_timer)
  elseif switch_name == "door_switch" then
    map:move_camera(376, 384, 250, door_timer)
  elseif switch_name == "final_barrier_switch" then
    map:set_entities_enabled("final_barrier", false)
    sol.audio.play_sound("secret")
  end
end

function barrier_timer()
  barrier:set_enabled(false)
  sol.audio.play_sound("secret")
  map:get_game():set_boolean(69, true)
end

function door_timer()
  map:open_doors("door")
  sol.audio.play_sound("secret")
end

function map:on_door_open(door_name)

  if door_name == "weak_block" then
    sol.audio.play_sound("secret")
  end
end

