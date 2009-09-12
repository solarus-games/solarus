-----------------------------
-- Outside world D2 script --
-----------------------------

function event_hero_on_sensor(sensor_name)

  if string.find(sensor_name, "not_in_demo_sensor") then
    start_message("outside_world.not_in_demo")
  end
end

