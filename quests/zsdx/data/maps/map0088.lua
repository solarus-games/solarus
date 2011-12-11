-- Castle B1

function event_map_started(destination_point_name)

  if destination_point_name == "from_outside_hole" then
    sol.map.hero_set_direction(2)
  end
end

function event_door_open(door_name)

  if door_name:find("^weak_wall") then
    sol.main.play_sound("secret")
  end
end

