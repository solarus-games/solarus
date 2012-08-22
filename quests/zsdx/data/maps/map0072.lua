local map = ...
-- Smith cave (battle against the thiefs)

function map:on_started(destination_point_name)

  map:set_doors_open("door", true)
  map:enemy_set_group_enabled("enemy", false)
end

function map:on_hero_on_sensor(sensor_name)

  if sensor_name == "close_door_sensor"
      and map:door_is_open("door")
      and not map:get_game():get_boolean(156) then
    map:close_doors("door")
    map:hero_freeze()
    sol.timer.start(1000, start_music)
  end
end

function start_music()

  sol.audio.play_music("soldiers")
  sol.timer.start(1500, start_fight)
end

function start_fight()

  map:enemy_set_group_enabled("enemy", true)
  map:hero_unfreeze()
end

function map:on_enemy_dead(enemy_name)

  if not map:has_entities("enemy") then
    map:hero_start_victory()
  end
end

function map:on_hero_victory_finished()

  map:get_game():set_boolean(156, true)
  map:get_hero():teleport(3, "out_smith_cave")
end

