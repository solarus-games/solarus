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
  map:set_entities_enabled("star_sensor_A", true)
  map:set_entities_enabled("star_sensor_B", false)
  map:set_entities_enabled("star_tile_A", true)
  map:set_entities_enabled("star_tile_B", false)
  map:set_entities_enabled("star_floor_A", true)
  map:set_entities_enabled("star_floot_B", false)
end

function map:on_started()
  -- Star tiles puzzle
  for _, sensor in ipairs(map:get_entities("star_sensor_A")) do
    sensor.on_activated = star_sensor_A_activated
  end
  for _, sensor in ipairs(map:get_entities("star_sensor_B")) do
    sensor.on_activated = star_sensor_B_activated
  end
end


