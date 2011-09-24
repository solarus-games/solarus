-- Outside world A2

camera_timer = ""

function event_map_started(destination_point_name)

  -- enable dark world
  if sol.game.savegame_get_boolean(905) then
    sol.main.play_music("dark_world.spc")
    sol.map.tileset_set(13)
    sol.map.tile_set_group_enabled("castle_east_bridge", false)
    sol.map.tile_set_group_enabled("castle_east_bridge_off", true)

    if sol.game.savegame_get_boolean(907) then
      sol.map.switch_set_activated("castle_door_switch", true)
    else
      sol.map.tile_set_enabled("castle_door", true)
    end
  else
    sol.map.tile_set_group_enabled("castle_east_bridge_off", false)
  end
end

function event_switch_activated(switch_name)

  if switch_name == "castle_door_switch" then
    sol.map.camera_move(296, 552, 250)
    camera_timer = "open_castle_door"
  end
end

function event_camera_reached_target()

  sol.main.timer_start(1000, camera_timer, false)
end

function open_castle_door()

  sol.map.tile_set_enabled("castle_door", false)
  sol.game.savegame_set_boolean(907, true)
  sol.main.play_sound("secret")
  sol.main.play_sound("door_open")
  sol.main.timer_start(1000, "sol.map.camera_restore", false)
end

