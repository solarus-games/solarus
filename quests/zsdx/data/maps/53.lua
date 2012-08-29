local map = ...
-- Dungeon 8 hidden room

function map:on_opening_transition_finished(destination_point)
  if destination_point:get_name() == "from_billy_cave" then
    map:start_dialog("dungeon_8.welcome")
  end
end

