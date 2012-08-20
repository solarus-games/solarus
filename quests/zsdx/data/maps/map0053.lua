local map = ...
-- Dungeon 8 hidden room

function map:on_map_opening_transition_finished(destination_point_name)
  if destination_point_name == "from_billy_cave" then
    map:start_dialog("dungeon_8.welcome")
  end
end

