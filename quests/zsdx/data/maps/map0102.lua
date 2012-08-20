local map = ...
-- Dungeon 7 2F

fighting_miniboss = false
code_nb_activated = 0
code_next_index = 1

function map:on_started(destination_point_name)

  -- pipe maze
  map:wall_set_group_enabled("pipe_border", false)

  -- door of the pots and pikes
  map:door_set_open("door_f", true)
  map:switch_set_activated("door_f_switch", true)

  -- west enemies room
  map:door_set_open("door_c", true)
  if map:get_game():get_boolean(616) then
    local enemy_name = "w_room_enemy_4"
    local x, y = map:enemy_get_position(enemy_name)
    map:enemy_set_position(enemy_name, x, y, 1)
  end

  -- saved door A (code)
  if map:get_game():get_boolean(630) then
    for i = 1, 8 do
      map:switch_set_activated("code_switch_" .. i, true)
    end
  end

  -- saved door D
  if map:get_game():get_boolean(615) then
    map:switch_set_activated("door_d_switch", true)
  end

  -- torches
  if map:get_game():get_boolean(618) then
    lock_torches()
  end

  -- weak floor
  if map:get_game():get_boolean(619) then
    map:tile_set_group_enabled("weak_floor", false)
    map:sensor_set_enabled("weak_floor_sensor", false)
  else
    map:teletransporter_set_enabled("weak_floor_teletransporter", false)
  end

  -- miniboss
  map:door_set_open("miniboss_door", true)
  map:enemy_set_group_enabled("miniboss", false)

  -- save the north-west door from 1F
  if destination_point_name == "from_1f_ne" then 
    map:get_game():set_boolean(621, true)
  end

  -- block fallen from 3F
  if not map:get_game():get_boolean(623) then
    map:tile_set_enabled("from_hole_a_tile", false)
    map:block_set_enabled("from_hole_a_block", false)
  end

  -- shortcut to the boss
  local shortcut = map:get_game():get_boolean(628)
  map:switch_set_activated("shortcut_switch", shortcut)
  map:tile_set_group_enabled("shortcut_on", shortcut)
  map:tile_set_group_enabled("shortcut_off", not shortcut)
end

function map:on_hero_on_sensor(sensor_name)

  -- close door F
  if sensor_name == "close_door_f_sensor" then
    map:door_set_open("door_f", false)
    map:switch_set_activated("door_f_switch", false)
 
  -- door C (west room)
  elseif sensor_name:find("^close_door_c_sensor") then
    if not map:enemy_is_group_dead("w_room_enemy")
        and map:door_is_open("door_c") then
      map:door_close("door_c")
    end

  -- miniboss
  elseif sensor_name == "start_miniboss_sensor"
      and not map:get_game():get_boolean(620)
      and not fighting_miniboss then

    map:hero_freeze()
    map:door_close("miniboss_door")
    fighting_miniboss = true
    sol.timer.start(1000, function()
      sol.audio.play_music("boss")
      map:enemy_set_group_enabled("miniboss", true)
      map:hero_unfreeze()
    end)

  -- pipes
  else
    pipe = string.match(sensor_name, "^pipe_in_([a-z])_sensor")
    if pipe ~= nil then
      -- entering a pipe
      map:wall_set_group_enabled("pipe_border_"..pipe, true)
      map:hero_set_visible(true)
    else
      pipe = string.match(sensor_name, "^pipe_out_([a-z])_sensor")
      if pipe ~= nil then
	-- leaving a pipe
	map:wall_set_group_enabled("pipe_border_"..pipe, false)
      elseif string.find(sensor_name, "^hide_hero_sensor") then
	-- hide the hero
	map:hero_set_visible(false)
      elseif string.find(sensor_name, "^unhide_hero_sensor") then
	-- unhide the hero
	map:hero_set_visible(true)
      end
    end
  end
end

function map:on_switch_activated(switch_name)

  -- door F
  if switch_name == "door_f_switch" then
    map:camera_move(1040, 760, 250, function()
      sol.audio.play_sound("secret")
      map:door_open("door_f")
    end)

  -- door D
  elseif switch_name == "door_d_switch" then
    sol.audio.play_sound("secret")
    map:door_open("door_d")

  -- shortcut to the boss
  elseif switch_name == "shortcut_switch" then
    map:tile_set_group_enabled("shortcut_on", true)
    map:tile_set_group_enabled("shortcut_off", false)
    map:get_game():set_boolean(628, true)
    sol.audio.play_sound("secret")
 
  -- code
  else
    local index = tonumber(string.match(switch_name, "^code_switch_([1-8])$"))
    if index ~= nil then
      if index == code_next_index then
	code_next_index = code_next_index + 1
      else
	code_next_index = 1
      end
      code_nb_activated = code_nb_activated + 1
      if code_nb_activated == 8 then
	-- the 8 switches are activated
        if code_next_index == 9 then
	  if not map:door_is_open("door_a") then
	    map:camera_move(72, 552, 250, function()
	      sol.audio.play_sound("secret")
	      map:door_open("door_a")
	    end)
	  else
	    sol.audio.play_sound("secret")
	  end
	else
	  sol.audio.play_sound("wrong")
	  for i = 1, 8 do
	    map:switch_set_activated("code_switch_" .. i, false)
	  end
	  code_nb_activated = 0

	  -- make sure the switch index won't get reactivated right now
	  map:switch_set_locked(switch_name, true)
	  sol.timer.start(500, function()
	    map:switch_set_locked(switch_name, false)
	  end)
	end
      end
    end
  end
end

function map:on_door_open(door_name)

  if door_name == "door_b" then
    -- put the last enemy of the room on the hero's layer
    local enemy_name = "w_room_enemy_4"
    if not map:enemy_is_dead(enemy_name) then
      local x, y = map:enemy_get_position(enemy_name)
      map:enemy_set_position(enemy_name, x, y, 1)
    end
  end
end

function map:on_enemy_dead(enemy_name)

  -- west enemies room
  if string.find(enemy_name, "^w_room_enemy")
      and map:enemy_is_group_dead("w_room_enemy") then
    sol.audio.play_sound("secret")
    if not map:door_is_open("door_c") then
      map:door_open("door_c")
    end
    if not map:door_is_open("door_a") then
      map:door_open("door_a")
    end

  -- miniboss
  elseif string.find(enemy_name, "^miniboss")
      and map:enemy_is_group_dead("miniboss") then

    sol.audio.play_music("dark_world_dungeon")
    sol.audio.play_sound("secret")
    map:door_open("miniboss_door")
    map:get_game():set_boolean(620, true)
  end
end

-- Returns whether all torches are on
function are_all_torches_on()

  return map:npc_exists("torch_1")
      and map:npc_get_sprite("torch_1"):get_animation() == "lit"
      and map:npc_get_sprite("torch_2"):get_animation() == "lit"
      and map:npc_get_sprite("torch_3"):get_animation() == "lit"
      and map:npc_get_sprite("torch_4"):get_animation() == "lit" 
end

-- Makes all torches on forever
function lock_torches()
  map:npc_remove("torch_1")
  map:npc_remove("torch_2")
  map:npc_remove("torch_3")
  map:npc_remove("torch_4")
end

function map:on_update()

  if not map:get_game():get_boolean(618)
    and are_all_torches_on() then

    lock_torches()
    map:camera_move(32, 120, 250, function()
      sol.audio.play_sound("secret")
      map:npc_set_position("nw_hint_stone", 32, 125)
    end)
  end
end

function map:on_sensor_collision_explosion(sensor_name)

  if sensor_name == "weak_floor_sensor"
      and map:tile_is_enabled("weak_floor") then

    map:tile_set_group_enabled("weak_floor", false)
    map:sensor_set_enabled("weak_floor_sensor", false)
    map:teletransporter_set_enabled("weak_floor_teletransporter", true)
    sol.audio.play_sound("secret")
    map:get_game():set_boolean(619, true)
  end
end

