-- Fairy cave 2F

function event_map_started(destination_point_name)

  if sol.game.savegame_get_boolean(953) then
    sol.map.tile_set_group_enabled("barrier", false)
    sol.map.switch_set_activated("barrier_switch", true)
  end
end

function event_switch_activated(switch_name)

  if switch_name == "barrier_switch" then
    sol.map.camera_move(312, 568, 250, function()
      sol.main.play_sound("secret")
      sol.game.savegame_set_boolean(953, true)
      sol.map.tile_set_group_enabled("barrier", false)
    end)
  end
end

