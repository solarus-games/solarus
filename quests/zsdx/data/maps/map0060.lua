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

  map:set_doors_open("boss_door", true)
  map:set_doors_open("final_room_door", true)
end

function map:on_opening_transition_finished(destination_point_name)

  -- show the welcome message
  if destination_point_name == "from_outside" then
    map:start_dialog("dungeon_4.welcome")
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
    map:close_doors("boss_door")
    map:close_doors("final_room_door")
    map:get_hero():freeze()
    sol.timer.start(1000, start_boss)
    fighting_boss = true
  end
end

function start_boss()

  sol.audio.play_music("boss")
  map:enemy_set_enabled("boss", true)
  map:get_hero():unfreeze()
end

function map:on_obtained_treasure(item_name, variant, savegame_variable)

  if item_name == "heart_container" then
    sol.timer.start(9000, open_final_room)
    sol.audio.play_music("victory")
    map:get_hero():freeze()
    map:get_hero():set_direction(3)
  end
end

function open_final_room()

  map:open_doors("final_room_door")
  sol.audio.play_sound("secret")
  map:get_hero():unfreeze()
end

