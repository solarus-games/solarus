-- Castle B1

function event_map_started(destination_point_name)

  if destination_point_name == "from_outside_hole" then
    sol.map.hero_set_direction(2)
  end
end

