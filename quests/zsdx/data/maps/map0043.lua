-- Dungeon 3 4F

function event_map_started(destination_point_name)

  sol.map.door_set_open("miniboss_door", true)
  sol.map.door_set_open("boss_door", true)
  if destination_point_name == "from_5f_c"
      or sol.game.savegame_get_boolean(903) then
    sol.map.door_set_open("final_room_door", true)
  end
end

fighting_miniboss = false
fighting_boss = false

function event_hero_on_sensor(sensor_name)

  if sensor_name == "start_miniboss_sensor"
      and not sol.game.savegame_get_boolean(901)
      and not fighting_miniboss then
    -- the miniboss is alive
    sol.map.door_close("miniboss_door")
    sol.map.hero_walk(666666, false, false)
  elseif sensor_name == "start_miniboss_sensor_2"
      and not sol.game.savegame_get_boolean(901)
      and not fighting_miniboss then
    sol.map.hero_freeze()
    sol.main.timer_start(miniboss_timer, 1000)
    fighting_miniboss = true
  elseif sensor_name == "start_boss_sensor"
      and not sol.game.savegame_get_boolean(902)
      and not fighting_boss then
    start_boss()
  end
end

function miniboss_timer()

  sol.main.play_music("boss.spc")
  sol.map.enemy_set_enabled("miniboss", true)
  sol.map.tile_set_group_enabled("miniboss_prickles", true)
  sol.map.hero_unfreeze()
end

function event_enemy_dead(enemy_name)

  if enemy_name == "miniboss" then
    sol.main.play_music("dark_world_dungeon.spc")
    sol.map.door_open("miniboss_door")
    sol.map.tile_set_group_enabled("miniboss_prickles", false)
  end
end

function start_boss()

  sol.map.door_close("boss_door")
  sol.main.play_sound("door_closed")
  sol.map.dialog_start("dungeon_3.arbror_hello")
end

function event_dialog_finished(first_message_id)

  if first_message_id == "dungeon_3.arbror_hello" then
    sol.main.play_music("boss.spc")
    fighting_boss = true
    sol.map.enemy_set_enabled("boss", true)
  end
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

