local map = ...

function map:on_hero_on_sensor(sensor_name)

  local layer = sensor_name:match("^to_layer_([0-1])_sensor")
  if layer ~= nil then
    local x, y = map:hero_get_position()
    map:hero_set_position(x, y, layer)
  end
end

