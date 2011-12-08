-- West mountains cave 1F

function event_map_started(destination_point_name)

  if destination_point_name == "from_2f_w" 
      or sol.game.is_dungeon_finished(7) then
      -- don't let the player go to dungeon 8 if dungeon 7 is not finished
      sol.map.door_set_open("nw_door", true)
  end
end

