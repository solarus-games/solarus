local map = ...
-- Lake shop

function map:on_started(destination_point_name)

  if destination_point_name == "from_outside_n"
      and not map:door_is_open("door") then
    -- not supposed to be possible but just to be safe, don't start stuck in the wall
    map:set_doors_open("door", true)
  end
end

function map:on_door_open(door_name)

  if door_name == "door" then
    sol.audio.play_sound("secret")
  end
end

