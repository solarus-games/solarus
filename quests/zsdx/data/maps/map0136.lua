local map = ...
-- Hidden palace D3

local holes_a_switches = map:get_entities("holes_a_switch")
local holes_b_switches = map:get_entities("holes_a_switch")

local function change_holes(enable_b)

  for _, switch in ipairs holes_a_switches do
    switch:set_activated(enable_b)
  end
  for _, switch in ipairs holes_b_switches do
    switch:set_activated(not enable_b)
  end
  map:set_entities_enabled("hole_a", not enable_b)
  map:set_entities_enabled("hole_b", enable_b)
end

function map:on_started(destination_point)

  change_holes(false)
end

for _, switch in ipairs(holes_a_switches) do
  switch.on_activated = holes_a_switch_activated
end
function holes_a_switch_activated(switch)
  sol.audio.play_sound("secret")
  change_holes(true)
end

for _, switch in ipairs(holes_b_switches) do
  switch.on_activated = holes_b_switch_activated
end
function holes_b_switch_activated(switch)
  sol.audio.play_sound("secret")
  change_holes(false)
end

local function save_solid_ground(sensor)
  hero:save_solid_ground()
end

for _, sensor in ipairs(map:get_entities("save_solid_ground_sensor")) do
  sensor.on_activated = save_solid_ground
end

