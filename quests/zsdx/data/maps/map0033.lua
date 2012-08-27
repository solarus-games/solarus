local map = ...
-- Cave of the hole near the flower

function map:on_started(destination_point)

  if destination_point:get_name() ~= "main_entrance" then
    map:set_doors_open("door", true)
  end
end

function map:on_hero_on_sensor(sensor_name)

  if sensor_name == "close_door_sensor"
      and door:is_open() then
    map:close_doors("door")
  end
end

