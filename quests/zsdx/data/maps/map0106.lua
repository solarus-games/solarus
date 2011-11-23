-- Dungeon 9 2F

function event_map_opening_transition_finished(destination_point_name)

  -- show the welcome message
  if destination_point_name:find("^from_outside") then
    sol.map.dialog_start("dungeon_9.welcome")
  end
end

