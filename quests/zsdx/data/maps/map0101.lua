local map = ...
-- Dungeon 7 1F

next_sign = 1
directions = {
  2, 1, 0, 3, 0, 3, 2, 1, 0, 3, 2, 3, 0, 1,
  2, 1, 0, 3, 0, 3, 0, 1, 2, 3, 2, 1, 2  
}
door_a_allow_close = true
door_b_allow_close = true

function map:on_started(destination_point_name)

  map:tile_set_group_enabled("pipe_over", false)
  map:tile_set_group_enabled("pipe_under", true)
  map:wall_set_group_enabled("pipe_border", false)

  if map:get_game():get_boolean(621) then
    map:switch_set_activated("ne_door_switch", true)
    map:switch_set_locked("ne_door_switch", true)
  end
end

function map:on_opening_transition_finished(destination_point_name)

  -- show the welcome message
  if destination_point_name == "from_outside" then
    map:start_dialog("dungeon_7.welcome")
  end
end

function map:on_hero_on_sensor(sensor_name)

  -- door A (south-west timed door)
  if sensor_name:find("^door_a_dont_close_sensor") then
    -- disable the timer that would close the timed door
    door_a_allow_close = false
  elseif sensor_name:find("^door_a_close_sensor")
      and map:door_is_open("door_a") then
    door_a_allow_close = true
    if not map:switch_is_activated("door_a_switch") then
      -- the timer has expired in the meantime
      map:close_doors("door_a")
    end

  -- door B (south-east timed door)
  elseif sensor_name:find("^door_b_dont_close_sensor") then
    -- disable the timer that would close the timed door
    door_b_allow_close = false
  elseif sensor_name:find("^door_b_close_sensor")
      and map:door_is_open("door_b") then
    door_a_allow_close = true
    if not map:switch_is_activated("door_b_switch") then
      -- the timer has expired in the meantime
      map:close_doors("door_b")
    end

  -- pipes
  else
    pipe = string.match(sensor_name, "^pipe_in_([a-z])_sensor")
    if pipe ~= nil then
      -- entering a pipe
      map:tile_set_group_enabled("pipe_under_"..pipe, false)
      map:tile_set_group_enabled("pipe_over_"..pipe, true)
      map:wall_set_group_enabled("pipe_border_"..pipe, true)
      map:get_hero():set_visible(true)
    else
      pipe = string.match(sensor_name, "^pipe_out_([a-z])_sensor")
      if pipe ~= nil then
	-- leaving a pipe
	map:tile_set_group_enabled("pipe_under_"..pipe, true)
	map:tile_set_group_enabled("pipe_over_"..pipe, false)
	map:wall_set_group_enabled("pipe_border_"..pipe, false)
      elseif string.find(sensor_name, "^hide_hero_sensor") then
	-- hide the hero
	map:get_hero():set_visible(false)
      elseif string.find(sensor_name, "^unhide_hero_sensor") then
	-- unhide the hero
	map:get_hero():set_visible(true)
      end
    end
  end
end

function map:on_npc_interaction(npc_name)

  -- sign maze
  if string.find(npc_name, "^sign_") and not map:door_is_open("sw_door") then
    if npc_name == "sign_"..next_sign then

      if next_sign <= #directions then
        map:start_dialog("surprise_wall.direction_" .. directions[next_sign])
      elseif next_sign == #directions + 1 then
	map:move_camera(376, 984, 250, function()
	  sol.audio.play_sound("secret")
	  map:open_doors("sw_door")
	end)
      end
      next_sign = next_sign + 1
    else
      sol.audio.play_sound("wrong")
      next_sign = 1
    end
  end
end

function map:on_switch_activated(switch_name)

  -- door A
  if switch_name == "door_a_switch" 
      and not map:door_is_open("door_a") then
    -- open the door and close it in a few seconds
    sol.audio.play_sound("secret")
    map:open_doors("door_a")
    door_a_allow_close = true
    sol.timer.start(10000, true, function()
      map:switch_set_activated("door_a_switch", false)
      if door_a_allow_close then
	map:close_doors("door_a")
      end
    end)

  -- door B
  elseif switch_name == "door_b_switch" 
      and not map:door_is_open("door_b") then
    -- open the door and close it in a few seconds
    sol.audio.play_sound("secret")
    map:open_doors("door_b")
    door_b_allow_close = true
    sol.timer.start(15000, true, function()
      map:switch_set_activated("door_b_switch", false)
      if door_b_allow_close then
	map:close_doors("door_b")
      end
    end)

  -- north-west door
  elseif switch_name == "ne_door_switch" then
    map:open_doors("ne_door")
  end
end

function map:on_switch_inactivated(switch_name)

  if switch_name == "ne_door_switch" then
    map:close_doors("ne_door")
  end
end

