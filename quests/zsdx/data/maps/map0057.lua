-- Dungeon 6 4F

fighting_boss = false

function event_map_started(destination_point_name)

  sol.map.door_set_open("ne_door", true)
  sol.map.door_set_open("boss_door", true)
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "ne_door_sensor" then

    if sol.map.door_is_open("ne_door") then
      sol.map.door_close("ne_door")
    else
      sol.map.door_open("ne_door")
    end
  elseif string.find(sensor_name, "start_boss_sensor")
      and not sol.game.savegame_get_boolean(321)
      and not fighting_boss then

    sol.map.sensor_set_group_enabled("start_boss_sensor", false)
    start_boos()
  end
end

function start_boos()

  sol.map.hero_freeze()
  sol.map.door_close("boss_door")
  sol.main.timer_start(function()

    sol.main.play_music("boss.spc")
    sol.map.enemy_set_enabled("boss", true)
    sol.map.hero_unfreeze()
    fighting_boss = true

  end,
  1000)
end

