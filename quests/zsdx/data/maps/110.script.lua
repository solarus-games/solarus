local map = ...
-- Dungeon 9 6F

function puzzle_end_sensor:on_activated()

  if not map:get_game():get_boolean(816) then
    -- the end: make a shortcut from 1F
    sol.audio.play_sound("secret")
    map:get_game():set_boolean(816, true)
  end
end

local function wrong_sensor_activated(sensor)

  -- wrong room: go back to the beginning
  local direction = tonumber(sensor:get_name():match("^wrong_sensor_([0-3])"))
  if direction == 0 then
    hero:set_position(1328, 1677)
  elseif direction == 1 then
    hero:set_position(1448, 1765)
  elseif direction == 2 then
    hero:set_position(1568, 1677)
  elseif direction == 3 then
    hero:set_position(1448, 1589)
  end
end

for _, sensor in ipairs(map:get_entities("wrong_sensor_")) do
  sensor.on_activated = wrong_sensor_activated
end

