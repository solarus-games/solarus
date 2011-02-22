-- Dungeon 1 1F SE (Temple of Stupidities)

function event_map_opening_transition_finished(destination_point_name)

  -- show the welcome message
  if destination_point_name == "from_outside" then
    sol.map.dialog_start("dungeon_1.welcome")
  end
end

function event_door_open(door_name)

  if door_name == "weak_wall_red_tunic" then
    sol.main.play_sound("secret")
  end
end

