-- Dungeon 3 5F

function event_map_opening_transition_finished(destination_point_name)

  -- show the welcome message
  if destination_point_name == "from_above_tree" then
    sol.map.dialog_start("dungeon_3")
  end
end

