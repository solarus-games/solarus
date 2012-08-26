local map = ...
-- Dungeon 9 1F

-- puzzle B
puzzle_b_next = nil -- index of the next correct switch (nil = error or not started)
puzzle_b_nb_activated = 0

-- torches and bridges
nb_torches_lit = 0

function map:on_started(destination_point_name)

  -- hidden Gibdo and chest
  map:enemy_set_group_enabled("hidden_enemy", false)
  if not map:get_game():get_boolean(800) then
    map:chest_set_enabled("hidden_enemy_chest", false)
  end

  -- puzzle A
  if map:get_game():get_boolean(802) then
    -- already solved
    map:tile_set_group_enabled("puzzle_a_switch_red", false)
    map:switch_set_group_enabled("puzzle_a_switch", false)
  else
    -- not solved yet
    map:chest_set_enabled("puzzle_a_chest", false)
  end
  map:tile_set_group_enabled("puzzle_a_switch_green", false)

  -- puzzle B
  if destination_point_name == "from_b1_w"
      or destination_point_name == "from_b1_e" then
    map:set_doors_open("puzzle_b_door", true)
    map:switch_set_activated("puzzle_b_door_switch", true)
  end

  -- south door
  if destination_point_name ~= "from_outside" then
    map:set_doors_open("s_door", true)
  end

  -- bridges that appear when a torch is lit
  map:tile_set_group_enabled("bridge", false)

  -- west enemies room
  if not map:get_game():get_boolean(806) then
    map:chest_set_enabled("w_room_chest", false)
  else
    map:set_doors_open("w_room_door", true)
  end

  -- central room
  map:set_doors_open("c_door_e", true)
  map:set_doors_open("c_door_s", true)

  -- east enemies room
  if not map:get_game():get_boolean(808) then
    map:chest_set_enabled("e_room_chest", false)
  end

  -- north-west chest
  if not map:get_game():get_boolean(810) then
    map:chest_set_enabled("nw_chest", false)
  else
    map:switch_set_activated("nw_switch_1", true)
    map:switch_set_activated("nw_switch_2", true)
  end

  -- shortcut to the boss
  if not map:get_game():get_boolean(816) then
    map:tile_set_enabled("shortcut", false)
    map:teletransporter_set_enabled("shortcut_teletransporter", false)
  end
end

function map:on_opening_transition_finished(destination_point_name)

  -- show the welcome message
  if destination_point_name == "from_outside" then
    map:start_dialog("dungeon_9.welcome")
  end
end

function map:on_update()

  -- hidden enemies
  local x, y = map:get_hero():get_position()
  if x > 1056 and x < 1200 and y > 1888 and y < 1968
      and not map:chest_is_enabled("hidden_enemy_chest")
      and not map:enemy_is_dead("hidden_enemy_1") 
      and not map:enemy_is_dead("hidden_enemy_2")
      and not map:enemy_is_enabled("hidden_enemy_1")
      and not map:enemy_is_enabled("hidden_enemy_2") then

    sol.audio.play_sound("cane")
    map:enemy_set_enabled("hidden_enemy_1", true)
    map:enemy_set_enabled("hidden_enemy_2", true)
  end
end

function map:on_enemy_dead(enemy_name)

  -- hidden enemies
  if enemy_name:find("^hidden_enemy")
      and not map:has_entities("hidden_enemy")
      and not map:chest_is_enabled("hidden_enemy_chest") then
    map:move_camera(1128, 2040, 250, function()
      sol.audio.play_sound("chest_appears")
      map:chest_set_enabled("hidden_enemy_chest", true)
    end)

  -- south door
  elseif enemy_name:find("^s_door_enemy")
      and not map:has_entities("s_door_enemy")
      and not map:door_is_open("s_door") then
    map:move_camera(1768, 1800, 250, function()
      sol.audio.play_sound("secret")
      map:open_doors("s_door")
    end)
    
  -- west enemies room
  elseif enemy_name:find("^w_room_enemy")
      and not map:has_entities("w_room_enemy") then
    sol.audio.play_sound("chest_appears")
    map:chest_set_enabled("w_room_chest", true)
    if not map:door_is_open("w_room_door") then
      map:open_doors("w_room_door")
    end

  -- east enemies room
  elseif enemy_name:find("^e_room_enemy")
      and not map:has_entities("e_room_enemy")
      and not map:chest_is_enabled("e_room_chest") then
    map:move_camera(2136, 1120, 250, function()
      sol.audio.play_sound("chest_appears")
      map:chest_set_enabled("e_room_chest", true)
    end)
  end
end

function map:on_switch_activated(switch_name)

  -- door to puzzle B
  if switch_name == "puzzle_b_door_switch" then
    if not map:door_is_open("puzzle_b_door") then
      map:move_camera(808, 1544, 250, function()
	sol.audio.play_sound("secret")
	map:open_doors("puzzle_b_door")
	map:switch_set_activated("puzzle_b_door_switch", true)
      end)
    end
 
  -- north-west chest
  elseif switch_name:find("^nw_switch") then
    if not map:chest_is_enabled("nw_chest")
        and map:switch_is_activated("nw_switch_1")
        and map:switch_is_activated("nw_switch_2") then
      sol.audio.play_sound("chest_appears")
      map:chest_set_enabled("nw_chest", true)
    end

  -- central room
  elseif switch_name:find("^c_room_switch") then
    if map:switch_is_activated("c_room_switch_1")
        and map:switch_is_activated("c_room_switch_2")
        and map:switch_is_activated("c_room_switch_3")
        and map:switch_is_activated("c_room_switch_4") then
      sol.audio.play_sound("secret")
      map:open_doors("c_door")
    end

  -- puzzle B: the switches have to be activated clockwise
  elseif switch_name:find("^puzzle_b_switch") then

    local index = tonumber(switch_name:match("^puzzle_b_switch_([1-4])$"))
    if puzzle_b_nb_activated == 0 then
      -- first one
      puzzle_b_next = index
    end
  
    if index == puzzle_b_next then
      -- okay so far
      puzzle_b_next = puzzle_b_next % 4 + 1
    else
      -- error
      puzzle_b_next = nil
    end
    puzzle_b_nb_activated = puzzle_b_nb_activated + 1

    if puzzle_b_nb_activated == 4 then
      -- the 4 switches are on, was there an error?
      if puzzle_b_next == nil then
	-- error
	sol.audio.play_sound("wrong")
	for i = 1, 4 do
	  map:switch_set_activated("puzzle_b_switch_" .. i, false)
	end
	puzzle_b_nb_activated = 0
	map:switch_set_locked(switch_name, true)
	-- to avoid the switch to be activated again immediately
      else
	-- correct
	sol.audio.play_sound("secret")
	map:open_doors("puzzle_b_door")
        map:switch_set_activated("puzzle_b_door_switch", true)
      end
    end

  -- puzzle A: each switch changes its neighboors in the 4 main directions
  elseif switch_name:find("^puzzle_a_switch") then
    sol.audio.play_sound("switch")
    local to_change = { {2,4}, {1,3,5}, {2,6}, {1,5}, {2,4,6}, {3,5} }
    local index = tonumber(switch_name:match("^puzzle_a_switch_([1-6])$"))
    if index then

      -- invert the neighboors
      for i, v in ipairs(to_change[index]) do
	local on = map:tile_is_enabled("puzzle_a_switch_red_" .. v)
	map:tile_set_enabled("puzzle_a_switch_green_" .. v, on)
	map:tile_set_enabled("puzzle_a_switch_red_" .. v, not on)
      end
 
      -- check the success
      local success = true
      for i = 1, 6 do
	if map:tile_is_enabled("puzzle_a_switch_red_" .. i) then
	  success = false
	  break
	end
      end
      if success then
	map:tile_set_group_enabled("puzzle_a_switch", false)
	map:switch_set_group_enabled("puzzle_a_switch", false)
	map:move_camera(896, 1896, 250, function()
	  sol.audio.play_sound("chest_appears")
	  map:chest_set_enabled("puzzle_a_chest", true)
	  map:get_game():set_boolean(802, true)
	end)
      end
    end
  end
end

function map:on_switch_left(switch_name)

  if switch_name:find("^puzzle_b_switch") then
    map:switch_set_locked(switch_name, false)
  end
end

function map:on_hero_on_sensor(sensor_name)

  -- west room
  if sensor_name == "close_w_room_sensor" then

    if map:door_is_open("w_room_door")
	 and not map:chest_is_enabled("w_room_chest") then
      map:close_doors("w_room_door")
      map:create_enemy("w_room_enemy_1", "blue_pig_soldier", 1, 752, 877)
      map:create_enemy("w_room_enemy_2", "red_pig_soldier", 1, 808, 885)
      map:create_enemy("w_room_enemy_3", "blue_pig_soldier", 1, 864, 877)
    end
  elseif sensor_name == "open_w_room_sensor" then
    map:set_doors_open("w_room_door", true)

  -- central room
  elseif sensor_name:find("^close_c_doors_sensor")
      and map:door_is_open("c_door_e")
      and not map:switch_is_activated("c_room_switch_1") then
    if map:door_is_open("c_big_key_door") then
      map:close_doors("c_door_s")
    end
    map:close_doors("c_door_e")
    map:switch_set_activated("c_room_switch_1", false)
    map:switch_set_activated("c_room_switch_2", false)
    map:switch_set_activated("c_room_switch_3", false)
    map:switch_set_activated("c_room_switch_4", false)
  
  -- puzzle B
  elseif sensor_name:find("^close_puzzle_b_door_sensor") then

    if not map:switch_is_activated("puzzle_b_switch_1")
        and map:door_is_open("puzzle_b_door") then
      map:close_doors("puzzle_b_door")
      map:switch_set_activated("puzzle_b_door_switch", false)
    end

  -- save solid ground location
  elseif sensor_name:find("^save_solid_ground_sensor") then
    map:get_hero():save_solid_ground()

  -- reset solid ground location
  elseif sensor_name:find("^reset_solid_ground_sensor") then
    map:get_hero():reset_solid_ground()
  end
end

-- Torches on this map interact with the map script
-- because we don't want usual behavior from items/lamp.lua:
-- we want a shorter delay and we want torches to enable the bridge
function map:on_npc_interaction(npc_name)

  if string.find(npc_name, "^torch") then
    map:start_dialog("torch.need_lamp")
  end
end

-- Called when fire touches an NPC linked to this map
function map:on_npc_collision_fire(npc_name)

  if string.find(npc_name, "^torch") then
    
    local torch_sprite = map:npc_get_sprite(npc_name)
    if torch_sprite:get_animation() == "unlit" then
      -- temporarily light the torch up
      torch_sprite:set_animation("lit")
      if nb_torches_lit == 0 then
        map:tile_set_group_enabled("bridge", true)
      end
      nb_torches_lit = nb_torches_lit + 1
      sol.timer.start(4000, function()
        torch_sprite:set_animation("unlit")
        nb_torches_lit = nb_torches_lit - 1
        if nb_torches_lit == 0 then
	  map:tile_set_group_enabled("bridge", false)
	end
      end)
    end
  end
end

