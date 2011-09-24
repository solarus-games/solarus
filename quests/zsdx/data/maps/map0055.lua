-- Dungeon 6 2F

function event_hero_on_sensor(sensor_name)

  -- fix the position of the hero when he lands on barriers after the feather jump
  if sensor_name == "above_barrier_sensor" then
    local x, y = sol.map.hero_get_position()
    if y > 629 and y < 641 then
      sol.map.hero_set_position(x, 629, 0)
    elseif y >= 641 and y < 653 then
      sol.map.hero_set_position(x, 653, 0)
    end
  end
end

