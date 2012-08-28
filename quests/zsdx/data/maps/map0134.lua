local map = ...
-- Lake west cave

function map:on_started(destination_point)

  if destination_point:get_name() == "from_outside_s" then
    map:set_doors_open("door", true)
    door_switch:set_activated(true)
    door_switch:set_locked(true)
  end
end

function door_switch:on_activated()
  map:open_doors("door")
end

function door_switch:on_inactivated()
  map:close_doors("door")
end

