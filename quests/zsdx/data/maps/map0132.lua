local map = ...

function map:on_hero_on_sensor(sensor_name)

  if sensor_name:find("^save_solid_ground_sensor") then
    hero:save_solid_ground()
  end
end

