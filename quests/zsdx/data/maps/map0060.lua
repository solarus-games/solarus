-- Dungeon 4 1F

fighting_boss = false

function event_map_started(destination_point_name)

  if sol.map.get_game():get_boolean(412) then
    sol.map.tile_set_enabled("weak_floor", false)
    sol.map.sensor_set_enabled("weak_floor_sensor", false)
  else
    sol.map.teletransporter_set_enabled("weak_floor_teletransporter", false)
  end

  sol.map.door_set_open("boss_door", true)
  sol.map.door_set_open("final_room_door", true)
end

function event_map_opening_transition_finished(destination_point_name)

  -- show the welcome message
  if destination_point_name == "from_outside" then
    sol.map.dialog_start("dungeon_4.welcome")
  end
end

function event_door_open(door_name)

  if door_name == "hidden_weak_wall" or door_name == "weak_block" then
    sol.audio.play_sound("secret")
  end
end

function event_sensor_collision_explosion(sensor_name)

  if sensor_name == "weak_floor_sensor"
      and sol.map.tile_is_enabled("weak_floor") then

    sol.map.tile_set_enabled("weak_floor", false)
    sol.map.sensor_set_enabled("weak_floor_sensor", false)
    sol.map.teletransporter_set_enabled("weak_floor_teletransporter", true)
    sol.audio.play_sound("secret")
    sol.map.get_game():set_boolean(412, true)
  end
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "start_boss_sensor"
      and not sol.map.get_game():get_boolean(415)
      and not fighting_boss then
    sol.map.door_close("boss_door")
    sol.map.door_close("final_room_door")
    sol.map.hero_freeze()
    sol.main:start_timer(1000, start_boss)
    fighting_boss = true
  end
end

function start_boss()

  sol.audio.play_music("boss")
  sol.map.enemy_set_enabled("boss", true)
  sol.map.hero_unfreeze()
end

function event_treasure_obtained(item_name, variant, savegame_variable)

  if item_name == "heart_container" then
    sol.main:start_timer(9000, open_final_room)
    sol.audio.play_music("victory")
    sol.map.hero_freeze()
    sol.map.hero_set_direction(3)
  end
end

function open_final_room()

  sol.map.door_open("final_room_door")
  sol.audio.play_sound("secret")
  sol.map.hero_unfreeze()
end

