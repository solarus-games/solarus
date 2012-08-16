local map = ...
-- Dungeon 4 1F

fighting_boss = false

function map:on_started(destination_point_name)

  if map:get_game():get_boolean(412) then
    map:tile_set_enabled("weak_floor", false)
    map:sensor_set_enabled("weak_floor_sensor", false)
  else
    map:teletransporter_set_enabled("weak_floor_teletransporter", false)
  end

  map:door_set_open("boss_door", true)
  map:door_set_open("final_room_door", true)
end

function map:on_map_opening_transition_finished(destination_point_name)

  -- show the welcome message
  if destination_point_name == "from_outside" then
    map:dialog_start("dungeon_4.welcome")
  end
end

function map:on_door_open(door_name)

  if door_name == "hidden_weak_wall" or door_name == "weak_block" then
    sol.audio.play_sound("secret")
  end
end

function map:on_sensor_collision_explosion(sensor_name)

  if sensor_name == "weak_floor_sensor"
      and map:tile_is_enabled("weak_floor") then

    map:tile_set_enabled("weak_floor", false)
    map:sensor_set_enabled("weak_floor_sensor", false)
    map:teletransporter_set_enabled("weak_floor_teletransporter", true)
    sol.audio.play_sound("secret")
    map:get_game():set_boolean(412, true)
  end
end

function map:on_hero_on_sensor(sensor_name)

  if sensor_name == "start_boss_sensor"
      and not map:get_game():get_boolean(415)
      and not fighting_boss then
    map:door_close("boss_door")
    map:door_close("final_room_door")
    map:hero_freeze()
    sol.timer.start(1000, start_boss)
    fighting_boss = true
  end
end

function start_boss()

  sol.audio.play_music("boss")
  map:enemy_set_enabled("boss", true)
  map:hero_unfreeze()
end

function map:on_treasure_obtained(item_name, variant, savegame_variable)

  if item_name == "heart_container" then
    sol.timer.start(9000, open_final_room)
    sol.audio.play_music("victory")
    map:hero_freeze()
    map:hero_set_direction(3)
  end
end

function open_final_room()

  map:door_open("final_room_door")
  sol.audio.play_sound("secret")
  map:hero_unfreeze()
end

