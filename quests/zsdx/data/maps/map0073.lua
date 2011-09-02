-- Lake shop

function event_map_started(destination_point_name)

  if destination_point_name == "from_outside_n"
      and not sol.map.door_is_open("door") then
    -- not supposed to be possible but just to be safe, don't start stuck in the wall
    sol.map.door_set_open("door", true)
  end
end

function event_door_open(door_name)

  if door_name == "door" then
    sol.main.play_sound("secret")
  end
end

