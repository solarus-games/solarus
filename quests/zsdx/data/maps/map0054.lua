local map = ...
-- Dungeon 3 5F

function map:on_opening_transition_finished(destination_point_name)

  -- show the welcome message
  if destination_point_name == "from_above_tree" then
    map:start_dialog("dungeon_3")
  end
end

