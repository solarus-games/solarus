-------------------------
-- Link's house script --
-------------------------

function event_map_started(destination_point_name)

  if destination_point_name == "from_intro" then
    -- the intro scene is playing
    hud_set_enabled(true)
  end
end

