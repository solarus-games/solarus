-- Dungeon 1 1F SE (Stupidities Temple)

function event_map_started(destination_point_name)
--  sol.map.light_set(0)
end

function event_map_opening_transition_finished(destination_point_name)

  -- show the welcome message
  if destination_point_name == "from_outside" then
    sol.map.dialog_start("dungeon_1.welcome")
  end
end


