local map = ...

local function star_sensor_A_activated()
  sol.audio.play_sound("secret")
  map:set_entities_enabled("star_sensor_A", false)
  map:set_entities_enabled("star_sensor_B", true)
  map:set_entities_enabled("star_tile_A", false)
  map:set_entities_enabled("star_tile_B", true)
  map:set_entities_enabled("star_floor_A", false)
  map:set_entities_enabled("star_floor_B", true)
end

local function star_sensor_B_activated()
  sol.audio.play_sound("secret")
  for _, sensor in ipairs(map:get_entities("star_sensor_A")) do
    sensor:set_enabled(true)
  end
  for _, sensor in ipairs(map:get_entities("star_sensor_B")) do
    sensor:set_enabled(false)
  end
  for _, tile in ipairs(map:get_entities("star_tile_A")) do
    tile:set_enabled(true)
  end
  for _, tile in ipairs(map:get_entities("star_tile_B")) do
    tile:set_enabled(false)
  end
  for _, floor in ipairs(map:get_entities("star_floor_A")) do
    floor:set_enabled(true)
  end
  for _, floor in ipairs(map:get_entities("star_floor_B")) do
    floor:set_enabled(false)
  end
end

function map:on_started()
  for _, sensor in ipairs(map:get_entities("star_sensor_A")) do
    sensor.on_activated = star_sensor_A_activated
  end
  for _, sensor in ipairs(map:get_entities("star_sensor_B")) do
    sensor.on_activated = star_sensor_B_activated
  end
end


