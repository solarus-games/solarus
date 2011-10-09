-- Dungeon 4 1F

fighting_miniboss = false
fighting_boss = false

function event_map_started(destination_point_name)

  if sol.game.savegame_get_boolean(412) then
    sol.map.tile_set_enabled("weak_floor", false)
    sol.map.sensor_set_enabled("weak_floor_sensor", false)
  else
    sol.map.teletransporter_set_enabled("weak_floor_teletransporter", false)
  end

  -- boos and miniboss doors
  sol.map.door_set_open("miniboss_door", true)
  sol.map.door_set_open("boss_door", true)
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
    sol.map.sensor_set_enabled("weak_floor_sensor", false)
    sol.map.teletransporter_set_enabled("weak_floor_teletransporter", true)
    sol.main.play_sound("secret")
    sol.game.savegame_set_boolean(412, true)
  end
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "start_miniboss_sensor"
      and not sol.game.savegame_get_boolean(414)
      and not fighting_miniboss then
    sol.map.door_close("miniboss_door")
    sol.map.hero_freeze()
    sol.main.timer_start(miniboss_timer, 1000)
    fighting_miniboss = true
  elseif sensor_name == "start_boss_sensor"
      and not sol.game.savegame_get_boolean(415)
      and not fighting_boss then
    start_boss()
  end
end

function miniboss_timer()
  sol.main.play_music("boss.spc")
  sol.map.enemy_set_enabled("miniboss", true)
  sol.map.hero_unfreeze()
end

function event_enemy_dead(enemy_name)

  if enemy_name == "miniboss" then
    sol.main.play_music("light_world_dungeon.spc")
    sol.map.door_open("miniboss_door")
  end
end

function start_boss()

  fighting_boss = true
  sol.map.enemy_set_enabled("boss", true)
  sol.map.door_close("boss_door")
  sol.main.play_music("boss.spc")
  sol.main.play_sound("door_closed")
end

function event_treasure_obtained(item_name, variant, savegame_variable)

  if item_name == "heart_container" then
    sol.main.timer_start(open_final_room, 9000)
    sol.main.play_music("victory.spc")
    sol.map.hero_freeze()
    sol.map.hero_set_direction(3)
  end
end

function open_final_room()

  sol.map.door_open("final_room_door")
  sol.main.play_sound("secret")
  sol.map.hero_unfreeze()
end

