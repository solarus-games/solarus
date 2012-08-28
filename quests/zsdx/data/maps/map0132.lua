local map = ...

-- Laser cave.

function save_solid_ground_sensor:on_activated()
  hero:save_solid_ground()
end

