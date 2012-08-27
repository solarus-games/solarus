local map = ...
-- Hidden palace D3

local nb_switches_a = 6
local nb_switches_b = 3

function map:on_started(destination_point)

  change_holes(false)
end

function map:on_switch_activated(switch_name)

  if switch_name:find("^holes_a_switch") then
    sol.audio.play_sound("secret")
    change_holes(true)
  elseif switch_name:find("^holes_b_switch") then
    sol.audio.play_sound("secret")
    change_holes(false)
  end
end

function change_holes(enable_b)

  for i = 1, nb_switches_a do
    map:get_entity("holes_a_switch_" .. i):set_activated(enable_b)
  end
  for i = 1, nb_switches_b do
    map:get_entity("holes_b_switch_" .. i):set_activated(not enable_b)
  end
  map:set_entities_enabled("hole_a", not enable_b)
  map:set_entities_enabled("hole_b", enable_b)
end

function map:on_hero_on_sensor(sensor_name)

  if sensor_name:find("^save_solid_ground_sensor") then
    map:get_hero():save_solid_ground()
  end
end

