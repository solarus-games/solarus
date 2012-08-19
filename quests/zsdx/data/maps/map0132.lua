local map = ...

function map:on_hero_on_sensor(sensor_name)

  if sensor_name:find("^save_solid_ground_sensor") then
    map:hero_save_solid_ground()
  end
end

