-- Dungeon 9 6F

function event_hero_on_sensor(sensor_name)

  if sensor_name == "puzzle_end_sensor" then
    -- the end: make a shortcut from 1F
    sol.main.play_sound("secret")
    sol.game.savegame_set_boolean(816, true)

  else
    -- wrong room: go back to the beginning
    local direction = tonumber(sensor_name:match("^wrong_sensor_([0-3])"))
    if direction == 0 then
      sol.map.hero_set_position(1328, 1677)
    elseif direction == 1 then
      sol.map.hero_set_position(1448, 1765)
    elseif direction == 2 then
      sol.map.hero_set_position(1568, 1677)
    elseif direction == 3 then
      sol.map.hero_set_position(1448, 1589)
    end
  end
end

