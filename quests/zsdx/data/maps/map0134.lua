-- Lake west cave

function event_map_started(destination_point_name)

  if destination_point_name == "from_outside_s" then
    sol.map.door_set_open("door", true)
    sol.map.switch_set_activated("door_switch", true)
    sol.map.switch_set_locked("door_switch", true)
  end
end

function event_switch_activated(switch_name)

  if switch_name == "door_switch" then
    sol.map.door_open("door")
  end
end

function event_switch_inactivated(switch_name)

  if switch_name == "door_switch" then
    sol.map.door_close("door")
  end
end

