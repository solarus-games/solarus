-- Dungeon 7 1F

next_sign = 1
directions = {
  2, 1, 0, 3, 0, 3, 2, 1, 0, 3, 2, 3, 0, 1,
  2, 1, 0, 3, 0, 3, 0, 1, 2, 3, 2, 1, 2  
}
door_a_allow_close = true
door_b_allow_close = true

function event_map_started(destination_point_name)

  sol.map.tile_set_group_enabled("pipe_over", false)
  sol.map.tile_set_group_enabled("pipe_under", true)
  sol.map.obstacle_set_group_enabled("pipe_border", false)

  if sol.game.savegame_get_boolean(621) then
    sol.map.switch_set_activated("ne_door_switch", true)
    sol.map.switch_set_locked("ne_door_switch", true)
  end
end

function event_map_opening_transition_finished(destination_point_name)

  -- show the welcome message
  if destination_point_name == "from_outside" then
    sol.map.dialog_start("dungeon_7.welcome")
  end
end

function event_hero_on_sensor(sensor_name)

  -- door A (south-west timed door)
  if sensor_name:find("^door_a_dont_close_sensor") then
    -- disable the timer that would close the timed door
    door_a_allow_close = false
  elseif sensor_name:find("^door_a_close_sensor")
      and sol.map.door_is_open("door_a") then
    door_a_allow_close = true
    if not sol.map.switch_is_activated("door_a_switch") then
      -- the timer has expired in the meantime
      sol.map.door_close("door_a")
    end

  -- door B (south-east timed door)
  elseif sensor_name:find("^door_b_dont_close_sensor") then
    -- disable the timer that would close the timed door
    door_b_allow_close = false
  elseif sensor_name:find("^door_b_close_sensor")
      and sol.map.door_is_open("door_b") then
    door_a_allow_close = true
    if not sol.map.switch_is_activated("door_b_switch") then
      -- the timer has expired in the meantime
      sol.map.door_close("door_b")
    end

  -- pipes
  else
    pipe = string.match(sensor_name, "^pipe_in_([a-z])_sensor")
    if pipe ~= nil then
      -- entering a pipe
      sol.map.tile_set_group_enabled("pipe_under_"..pipe, false)
      sol.map.tile_set_group_enabled("pipe_over_"..pipe, true)
      sol.map.obstacle_set_group_enabled("pipe_border_"..pipe, true)
      sol.map.hero_set_visible(true)
    else
      pipe = string.match(sensor_name, "^pipe_out_([a-z])_sensor")
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
end

function event_npc_interaction(npc_name)

  -- sign maze
  if string.find(npc_name, "^sign_") and not sol.map.door_is_open("sw_door") then
    if npc_name == "sign_"..next_sign then

      if next_sign <= #directions then
        sol.map.dialog_start("surprise_wall.direction_" .. directions[next_sign])
      elseif next_sign == #directions + 1 then
	sol.map.camera_move(376, 984, 250, function()
	  sol.main.play_sound("secret")
	  sol.map.door_set_open("sw_door", true)
	end)
      end
      next_sign = next_sign + 1
    else
      sol.main.play_sound("wrong")
      next_sign = 1
    end
  end
end

function event_switch_activated(switch_name)

  -- door A
  if switch_name == "door_a_switch" 
      and not sol.map.door_is_open("door_a") then
    -- open the door and close it in a few seconds
    sol.main.play_sound("secret")
    sol.map.door_open("door_a")
    door_a_allow_close = true
    sol.main.timer_start(function()
      sol.map.switch_set_activated("door_a_switch", false)
      if door_a_allow_close then
	sol.map.door_close("door_a")
      end
    end, 10000, true)

  -- door B
  elseif switch_name == "door_b_switch" 
      and not sol.map.door_is_open("door_b") then
    -- open the door and close it in a few seconds
    sol.main.play_sound("secret")
    sol.map.door_open("door_b")
    door_b_allow_close = true
    sol.main.timer_start(function()
      sol.map.switch_set_activated("door_b_switch", false)
      if door_b_allow_close then
	sol.map.door_close("door_b")
      end
    end, 15000, true)

  -- north-west door
  elseif switch_name == "ne_door_switch" then
    sol.map.door_open("ne_door")
  end
end

function event_switch_inactivated(switch_name)

  if switch_name == "ne_door_switch" then
    sol.map.door_close("ne_door")
  end
end

