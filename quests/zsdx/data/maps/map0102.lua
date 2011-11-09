-- Dungeon 7 2F

function event_map_started(destination_point_name)

  -- pipe maze
  sol.map.obstacle_set_group_enabled("pipe_border", false)

  -- door of the pots and pikes
  sol.map.door_set_open("door_f", true)
  sol.map.switch_set_activated("door_f_switch", true)

  -- saved door D
  if sol.game.savegame_get_boolean(615) then
    sol.map.switch_set_activated("door_d_switch", true)
  end
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "open_door_f_sensor" then
    sol.map.door_set_open("door_f", false)
    sol.map.switch_set_activated("door_f_switch", false)
  else
    pipe = string.match(sensor_name, "^pipe_in_([a-z])_sensor")
    if pipe ~= nil then
      -- entering a pipe
      sol.map.obstacle_set_group_enabled("pipe_border_"..pipe, true)
      sol.map.hero_set_visible(true)
    else
      pipe = string.match(sensor_name, "^pipe_out_([a-z])_sensor")
      if pipe ~= nil then
	-- leaving a pipe
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
end

function event_switch_activated(switch_name)

  if switch_name == "door_f_switch" then
    sol.map.camera_move(1040, 760, 250, function()
      sol.main.play_sound("secret")
      sol.map.door_open("door_f")
    end)
  elseif switch_name == "door_a_switch" then
    sol.main.play_sound("secret")
    sol.map.door_open("door_a")
  end
end

function event_door_open(door_name)

  if door_name == "door_b" then
    -- put the last enemy of the room on the hero's layer
    local enemy_name = "w_room_enemy_4"
    if not sol.map.enemy_is_dead(enemy_name) then
      local x, y = sol.map.enemy_get_position(enemy_name)
      sol.map.enemy_set_position(enemy_name, x, y, 1)
    end
  end
end

function event_enemy_dead(enemy_name)

  if string.find(enemy_name, "^w_room_enemy")
    and sol.map.enemy_is_group_dead("w_room_enemy")
    and not sol.map.door_is_open("door_c") then
    sol.map.camera_move(264, 504, 250, function()
      sol.main.play_sound("secret")
      sol.map.door_open("door_c")
    end)
  end
end

