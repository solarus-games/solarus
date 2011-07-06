-- Dungeon 4 1F

function event_map_started(destination_point_name)

  if sol.game.savegame_get_boolean(412) then
    sol.map.tile_set_enabled("weak_floor", false)
  else
    sol.map.teletransporter_set_enabled("weak_floor_teletransporter", false)
  end
end

function event_map_opening_transition_finished(destination_point_name)

  -- show the welcome message
  if destination_point_name == "from_outside" then
    sol.map.dialog_start("dungeon_4.welcome")
  end
end

function event_door_open(door_name)

  if door_name == "hidden_weak_wall" or door_name == "weak_block" then
    sol.main.play_sound("secret")
  end
end

function event_sensor_collision_explosion(sensor_name)

  if sensor_name == "weak_floor_sensor"
      and sol.map.tile_is_enabled("weak_floor") then

    sol.map.tile_set_enabled("weak_floor", false)
    sol.map.teletransporter_set_enabled("weak_floor_teletransporter", true)
    sol.main.play_sound("secret")
    sol.game.savegame_set_boolean(412, true)
  end
end

