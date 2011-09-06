-- Dungeon 3 4F

function event_map_started(destination_point_name)

  sol.map.door_set_open("miniboss_door", true)
end

fighting_miniboss = false

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
    sol.main.timer_start(1000, "miniboss_timer", false)
    fighting_miniboss = true
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

