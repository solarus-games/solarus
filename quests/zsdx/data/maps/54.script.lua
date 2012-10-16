local map = ...
-- Dungeon 3 5F

function map:on_opening_transition_finished(destination)

  -- show the welcome message
  if destination:get_name() == "from_above_tree" then
    map:start_dialog("dungeon_3")
  end
end

