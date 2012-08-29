local map = ...
-- West mountains cave 1F

function map:on_started(destination)

  if destination:get_name() == "from_2f_w"
      or map:get_game():is_dungeon_finished(7) then
      -- don't let the player go to dungeon 8 if dungeon 7 is not finished
      map:set_doors_open("nw_door", true)
  end
end

