-- Dungeon 5 B1

function event_map_started(destination_point_name)

  if destination_point_name == "from_outside_hole" then
    sol.map.hero_set_direction(2)
  end

  if not sol.game.savegame_get_boolean(907) then
    -- this door is open until the main entrance door of
    -- the castle is open
    sol.map.door_set_open("n_door", true)
  end
end

function event_map_opening_transition_finished(destination_point_name)

  -- show the welcome message
  if destination_point_name == "from_outside_hole" then
    sol.map.dialog_start("dungeon_5.welcome")
  end
end

