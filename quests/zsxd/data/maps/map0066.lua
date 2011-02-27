-- Temple of Stupidities 2F SW

function event_map_started(destination_point_name)

  -- water removed
  if sol.game.savegame_get_boolean(283) then
    sol.map.tile_set_group_enabled("water", false)
  end
end

