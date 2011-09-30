-- Dungeon 5 2F

function event_map_opening_transition_finished(destination_point_name)

  -- show the welcome message
  if destination_point_name == "from_roof"
      or destination_point_name == "from_outside_w"
      or destination_point_name == "from_outside_e" then
    sol.map.dialog_start("dungeon_5.welcome")
  end
end

