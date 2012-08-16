local map = ...
-- Castle B1

function map:on_started(destination_point_name)

  if destination_point_name == "from_outside_hole" then
    map:hero_set_direction(2)
  end
end

function map:on_door_open(door_name)

  if door_name:find("^weak_wall") then
    sol.audio.play_sound("secret")
  end
end

