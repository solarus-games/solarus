-- Dungeon 8 hidden room

function event_map_opening_transition_finished(destination_point_name)
  if destination_point_name == "from_billy_cave" then
    sol.map.dialog_start("dungeon_8.welcome")
  end
end

