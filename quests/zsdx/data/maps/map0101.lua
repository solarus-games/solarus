-- Dungeon 7 1F

function event_map_started(destination_point_name)

  sol.map.tile_set_group_enabled("pipe_over", false)
  sol.map.tile_set_group_enabled("pipe_under", true)
  sol.map.obstacle_set_group_enabled("pipe_border", false)
end

function event_hero_on_sensor(sensor_name)

  pipe = string.match(sensor_name, "^pipe_in_\([a-z]\)_sensor")
  if pipe ~= nil then
    -- entering a pipe
    sol.map.tile_set_group_enabled("pipe_under_"..pipe, false)
    sol.map.tile_set_group_enabled("pipe_over_"..pipe, true)
    sol.map.obstacle_set_group_enabled("pipe_border_"..pipe, true)
    sol.map.hero_set_visible(true)
  else
    pipe = string.match(sensor_name, "^pipe_out_\([a-z]\)_sensor")
    if pipe ~= nil then
      -- leaving a pipe
      sol.map.tile_set_group_enabled("pipe_under_"..pipe, true)
      sol.map.tile_set_group_enabled("pipe_over_"..pipe, false)
      sol.map.obstacle_set_group_enabled("pipe_border_"..pipe, false)
    elseif string.find(sensor_name, "^hide_hero_sensor") then
      -- hide the hero
      sol.map.hero_set_visible(false)
    elseif string.find(sensor_name, "^unhide_hero_sensor") then
      -- unhide the hero
      sol.map.hero_set_visible(true)
    end
  end
end

