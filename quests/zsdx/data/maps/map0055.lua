-- Dungeon 6 2F

function event_hero_on_sensor(sensor_name)

  if sensor_name == "above_barrier_sensor" then
    x, y = sol.map.hero_get_position()
    if y < 653 then
      sol.map.hero_set_position(x, 653, 0)
    end
  end
end

