local map = ...
-- Dungeon 7 2F

fighting_miniboss = false
code_nb_activated = 0
code_next_index = 1

function map:on_started(destination_point)

  -- pipe maze
  map:set_entities_enabled("pipe_border", false)

  -- door of the pots and pikes
  map:set_doors_open("door_f", true)
  door_f_switch:set_activated(true)

  -- west enemies room
  map:set_doors_open("door_c", true)
  if map:get_game():get_boolean(616) then
    local enemy_name = "w_room_enemy_4"
    local x, y = map:get_entity(enemy_name):get_position()
    map:get_entity(enemy_name):set_position(x, y, 1)
  end

  -- saved door A (code)
  if map:get_game():get_boolean(630) then
    for i = 1, 8 do
      map:get_entity("code_switch_" .. i):set_activated(true)
    end
  end

  -- saved door D
  if map:get_game():get_boolean(615) then
    door_d_switch:set_activated(true)
  end

  -- torches
  if map:get_game():get_boolean(618) then
    lock_torches()
  end

  -- weak floor
  if map:get_game():get_boolean(619) then
    map:set_entities_enabled("weak_floor", false)
    weak_floor_sensor:set_enabled(false)
  else
    weak_floor_teletransporter:set_enabled(false)
  end

  -- miniboss
  map:set_doors_open("miniboss_door", true)
  map:set_entities_enabled("miniboss", false)

  -- save the north-west door from 1F
  if destination_point:get_name() == "from_1f_ne" then
    map:get_game():set_boolean(621, true)
  end

  -- block fallen from 3F
  if not map:get_game():get_boolean(623) then
    from_hole_a_tile:set_enabled(false)
    from_hole_a_block:set_enabled(false)
  end

  -- shortcut to the boss
  local shortcut = map:get_game():get_boolean(628)
  shortcut_switch:set_activated(shortcut)
  map:set_entities_enabled("shortcut_on", shortcut)
  map:set_entities_enabled("shortcut_off", not shortcut)
end

function map:on_hero_on_sensor(sensor_name)

  -- close door F
  if sensor_name == "close_door_f_sensor" then
    map:set_doors_open("door_f", false)
    door_f_switch:set_activated(false)

  -- door C (west room)
  elseif sensor_name:find("^close_door_c_sensor") then
    if map:has_entities("w_room_enemy")
        and door_c:is_open() then
      map:close_doors("door_c")
    end

  -- miniboss
  elseif sensor_name == "start_miniboss_sensor"
      and not map:get_game():get_boolean(620)
      and not fighting_miniboss then

    hero:freeze()
    map:close_doors("miniboss_door")
    fighting_miniboss = true
    sol.timer.start(1000, function()
      sol.audio.play_music("boss")
      map:set_entities_enabled("miniboss", true)
      hero:unfreeze()
    end)

  -- pipes
  else
    pipe = string.match(sensor_name, "^pipe_in_([a-z])_sensor")
    if pipe ~= nil then
      -- entering a pipe
      map:set_entities_enabled("pipe_border_"..pipe, true)
      hero:set_visible(true)
    else
      pipe = string.match(sensor_name, "^pipe_out_([a-z])_sensor")
      if pipe ~= nil then
	-- leaving a pipe
	map:set_entities_enabled("pipe_border_"..pipe, false)
      elseif string.find(sensor_name, "^hide_hero_sensor") then
	-- hide the hero
	hero:set_visible(false)
      elseif string.find(sensor_name, "^unhide_hero_sensor") then
	-- unhide the hero
	hero:set_visible(true)
      end
    end
  end
end

function map:on_switch_activated(switch_name)

  -- door F
  if switch_name == "door_f_switch" then
    map:move_camera(1040, 760, 250, function()
      sol.audio.play_sound("secret")
      map:open_doors("door_f")
    end)

  -- door D
  elseif switch_name == "door_d_switch" then
    sol.audio.play_sound("secret")
    map:open_doors("door_d")

  -- shortcut to the boss
  elseif switch_name == "shortcut_switch" then
    map:set_entities_enabled("shortcut_on", true)
    map:set_entities_enabled("shortcut_off", false)
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
	  if not door_a:is_open() then
	    map:move_camera(72, 552, 250, function()
	      sol.audio.play_sound("secret")
	      map:open_doors("door_a")
	    end)
	  else
	    sol.audio.play_sound("secret")
	  end
	else
	  sol.audio.play_sound("wrong")
	  for i = 1, 8 do
	    map:get_entity("code_switch_" .. i):set_activated(false)
	  end
	  code_nb_activated = 0

	  -- make sure the switch index won't get reactivated right now
	  map:get_entity(switch_name):set_locked(true)
	  sol.timer.start(500, function()
	    map:get_entity(switch_name):set_locked(false)
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
      local x, y = map:get_entity(enemy_name):get_position()
      map:get_entity(enemy_name):set_position(x, y, 1)
    end
  end
end

function map:on_enemy_dead(enemy_name)

  -- west enemies room
  if string.find(enemy_name, "^w_room_enemy")
      and not map:has_entities("w_room_enemy") then
    sol.audio.play_sound("secret")
    if not door_c:is_open() then
      map:open_doors("door_c")
    end
    if not door_a:is_open() then
      map:open_doors("door_a")
    end

  -- miniboss
  elseif string.find(enemy_name, "^miniboss")
      and not map:has_entities("miniboss") then

    sol.audio.play_music("dark_world_dungeon")
    sol.audio.play_sound("secret")
    map:open_doors("miniboss_door")
    map:get_game():set_boolean(620, true)
  end
end

-- Returns whether all torches are on
function are_all_torches_on()

  return torch_1:exists()
      and torch_1:get_sprite():get_animation() == "lit"
      and torch_2:get_sprite():get_animation() == "lit"
      and torch_3:get_sprite():get_animation() == "lit"
      and torch_4:get_sprite():get_animation() == "lit"
end

-- Makes all torches on forever
function lock_torches()
  torch_1:remove()
  torch_2:remove()
  torch_3:remove()
  torch_4:remove()
end

function map:on_update()

  if not map:get_game():get_boolean(618)
    and are_all_torches_on() then

    lock_torches()
    map:move_camera(32, 120, 250, function()
      sol.audio.play_sound("secret")
      nw_hint_stone:set_position(32, 125)
    end)
  end
end

function map:on_sensor_collision_explosion(sensor_name)

  if sensor_name == "weak_floor_sensor"
      and weak_floor:is_enabled() then

    map:set_entities_enabled("weak_floor", false)
    weak_floor_sensor:set_enabled(false)
    weak_floor_teletransporter:set_enabled(true)
    sol.audio.play_sound("secret")
    map:get_game():set_boolean(619, true)
  end
end

