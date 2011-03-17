-- Temple of Stupidities 2F SW

function event_map_started(destination_point_name)

  -- water removed
  if sol.game.savegame_get_boolean(283) then
    sol.map.tile_set_group_enabled("water", false)
  end

  -- fight room
  sol.map.door_set_open("fight_door", true)
  if sol.game.savegame_get_boolean(244) then
    sol.map.enemy_remove_group("fight")
  else
    -- TODO sol.map.enemy_set_group_enabled
    sol.map.enemy_set_enabled("fight_1", false)
    sol.map.enemy_set_enabled("fight_2", false)
    sol.map.enemy_set_enabled("fight_3", false)
    sol.map.enemy_set_enabled("fight_4", false)
    sol.map.chest_set_enabled("fight_chest", false)
  end
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "fight_sensor" 
      and not sol.game.savegame_get_boolean(244)
      and sol.map.door_is_open("fight_door")
      and not sol.map.chest_is_enabled("fight_chest") then
    sol.map.door_close("fight_door")
    sol.map.enemy_set_enabled("fight_1", true)
    sol.map.enemy_set_enabled("fight_2", true)
    sol.map.enemy_set_enabled("fight_3", true)
    sol.map.enemy_set_enabled("fight_4", true)
  end
end

function event_enemy_dead(enemy_name)

  if string.match(enemy_name, "^fight")
      and sol.map.enemy_is_group_dead("fight") then

    sol.main.play_sound("chest_appears")
    sol.map.chest_set_enabled("fight_chest", true)
    sol.map.door_open("fight_door")
  end
end
