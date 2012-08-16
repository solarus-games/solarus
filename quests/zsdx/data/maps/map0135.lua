local map = ...
-- Hidden palace D7

function map:on_hero_on_sensor(sensor_name)

  if sensor_name:find("^pipe_in_sensor") then
    -- entering a pipe
    map:hero_set_visible(true)
  elseif sensor_name:find("^pipe_out_sensor") then
    -- leaving a pipe
  elseif sensor_name:find("^hide_hero_sensor") then
    -- hide the hero
    map:hero_set_visible(false)
  elseif sensor_name:find("^unhide_hero_sensor") then
    -- unhide the hero
    map:hero_set_visible(true)
  elseif sensor_name:find("^pipe_border_a_sensor") then
    -- enable border A
    map:obstacle_set_group_enabled("pipe_border_a", true)
    map:obstacle_set_group_enabled("pipe_border_b", false)
  elseif sensor_name:find("^pipe_border_b_sensor") then
    -- enable border B
    map:obstacle_set_group_enabled("pipe_border_b", true)
    map:obstacle_set_group_enabled("pipe_border_a", false)
  end
end

