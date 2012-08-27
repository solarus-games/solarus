local map = ...
-- Hidden palace D7

function map:on_hero_on_sensor(sensor_name)

  if sensor_name:find("^pipe_in_sensor") then
    -- entering a pipe
    hero:set_visible(true)
  elseif sensor_name:find("^pipe_out_sensor") then
    -- leaving a pipe
  elseif sensor_name:find("^hide_hero_sensor") then
    -- hide the hero
    hero:set_visible(false)
  elseif sensor_name:find("^unhide_hero_sensor") then
    -- unhide the hero
    hero:set_visible(true)
  elseif sensor_name:find("^pipe_border_a_sensor") then
    -- enable border A
    map:set_entities_enabled("pipe_border_a", true)
    map:set_entities_enabled("pipe_border_b", false)
  elseif sensor_name:find("^pipe_border_b_sensor") then
    -- enable border B
    map:set_entities_enabled("pipe_border_b", true)
    map:set_entities_enabled("pipe_border_a", false)
  end
end

