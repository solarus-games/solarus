local map = ...
-- Lake west cave

function map:on_started(destination_point)

  if destination_point:get_name() == "from_outside_s" then
    map:set_doors_open("door", true)
    door_switch:set_activated(true)
    map:switch_set_locked("door_switch", true)
  end
end

function map:on_switch_activated(switch_name)

  if switch_name == "door_switch" then
    map:open_doors("door")
  end
end

function map:on_switch_inactivated(switch_name)

  if switch_name == "door_switch" then
    map:close_doors("door")
  end
end

