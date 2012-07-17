-- Smith cave (battle against the thiefs)

function event_map_started(destination_point_name)

  sol.map.door_set_open("door", true)
  sol.map.enemy_set_group_enabled("enemy", false)
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "close_door_sensor"
      and sol.map.door_is_open("door")
      and not sol.map.get_game():get_boolean(156) then
    sol.map.door_close("door")
    sol.map.hero_freeze()
    sol.timer.start(1000, start_music)
  end
end

function start_music()

  sol.audio.play_music("soldiers")
  sol.timer.start(1500, start_fight)
end

function start_fight()

  sol.map.enemy_set_group_enabled("enemy", true)
  sol.map.hero_unfreeze()
end

function event_enemy_dead(enemy_name)

  if sol.map.enemy_is_group_dead("enemy") then
    sol.map.hero_start_victory_sequence()
  end
end

function event_hero_victory_sequence_finished()

  sol.map.get_game():set_boolean(156, true)
  sol.map.hero_set_map(3, "out_smith_cave", 1)
end

