-- Hidden palace D7

function event_hero_on_sensor(sensor_name)

  if sensor_name:find("^pipe_in_sensor") then
    -- entering a pipe
    sol.map.hero_set_visible(true)
  elseif sensor_name:find("^pipe_out_sensor") then
    -- leaving a pipe
  elseif sensor_name:find("^hide_hero_sensor") then
    -- hide the hero
    sol.map.hero_set_visible(false)
  elseif sensor_name:find("^unhide_hero_sensor") then
    -- unhide the hero
    sol.map.hero_set_visible(true)
  elseif sensor_name:find("^pipe_border_a_sensor") then
    -- enable border A
    sol.map.obstacle_set_group_enabled("pipe_border_a", true)
    sol.map.obstacle_set_group_enabled("pipe_border_b", false)
  elseif sensor_name:find("^pipe_border_b_sensor") then
    -- enable border B
    sol.map.obstacle_set_group_enabled("pipe_border_b", true)
    sol.map.obstacle_set_group_enabled("pipe_border_a", false)
  end
end

