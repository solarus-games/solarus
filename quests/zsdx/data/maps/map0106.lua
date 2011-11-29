-- Dungeon 9 2F

ne_puzzle_step = nil
chests_puzzle_step = nil
nw_switches_next = nil -- index of the next correct switch (nil = error or not started)
nw_switches_nb_activated = 0
nb_torches_lit = 0
door_g_finished = false

function event_map_started(destination_point_name)

  -- north barrier
  if sol.game.savegame_get_boolean(812) then
    sol.map.switch_set_activated("n_barrier_switch", true)
    sol.map.tile_set_enabled("n_barrier", false)
  end

  -- enemies rooms
  sol.map.door_set_open("door_b", true)
  sol.map.door_set_open("door_c", true)

  -- north-east room
  if destination_point_name == "from_3f_e" then
    sol.map.door_set_open("door_a", true)
    ne_puzzle_set_step(5)
  else
    ne_puzzle_set_step(1)
  end

  -- compass
  if sol.game.savegame_get_boolean(814) then
    for i = 1, 7 do
      sol.map.chest_set_open("compass_chest_" .. i, true)
    end
  else
    chests_puzzle_step = 1
  end

  -- clockwise switches and next doors
  if destination_point_name ~= "from_1f" then
    sol.map.door_set_open("door_d", true)
    sol.map.door_set_open("door_e", true)
    sol.map.switch_set_activated("door_e_switch", true)
    for i = 1, 8 do
      sol.map.switch_set_activated("nw_switch_" .. i, true)
    end
  end

  -- bridges that appear when a torch is lit
  sol.map.tile_set_group_enabled("bridge", false)
end

function event_map_opening_transition_finished(destination_point_name)

  -- show the welcome message
  if destination_point_name:find("^from_outside") then
    sol.map.dialog_start("dungeon_9.welcome")
  end
end

function event_switch_activated(switch_name)

  -- north barrier
  if switch_name == "n_barrier_switch" then
    sol.main.play_sound("secret")
    sol.main.play_sound("door_open")
    sol.map.tile_set_enabled("n_barrier", false)
    sol.game.savegame_set_boolean(812, true)

  -- door A
  elseif switch_name == "door_a_switch" then
    sol.main.play_sound("secret")
    sol.map.door_open("door_a")

  -- door E
  elseif switch_name == "door_e_switch" then
    sol.main.play_sound("secret")
    sol.map.door_open("door_e")

  -- door G
  elseif switch_name == "door_g_switch"
      and not sol.map.door_is_open("door_g") then
    sol.map.camera_move(1760, 520, 1000, function()
      sol.main.play_sound("secret")
      sol.map.door_open("door_g")
      door_g_finished = false
    end)

  -- north-west puzzle: the switches have to be activated clockwise
  elseif switch_name:find("^nw_switch") then

    local index = tonumber(switch_name:match("^nw_switch_([1-8])$"))
    if nw_switches_nb_activated == 0 then
      -- first one
      nw_switches_next = index
    end
  
    if index == nw_switches_next then
      -- okay so far
      nw_switches_next = nw_switches_next % 8 + 1
    else
      -- error
      nw_switches_next = nil
    end
    nw_switches_nb_activated = nw_switches_nb_activated + 1

    if nw_switches_nb_activated == 8 then
      -- the 8 switches are on, was there an error?
      if nw_switches_next == nil then
	-- error
	sol.main.play_sound("wrong")
	for i = 1, 8 do
	  sol.map.switch_set_activated("nw_switch_" .. i, false)
	end
	nw_switches_next = 0
	sol.map.switch_set_locked(switch_name, true)
	-- to avoid the switch to be activated again immediately
      else
	-- correct
	sol.main.play_sound("secret")
	sol.map.door_open("door_d")
      end
    end
  end
end

function event_switch_left(switch_name)

  if switch_name:find("^nw_switch") then
    sol.map.switch_set_locked(switch_name, false)
  end
end

function event_hero_on_sensor(sensor_name)

  -- north-east puzzle
  if sensor_name == "ne_puzzle_sensor_1" then

    sol.map.hero_set_position(2408, 653)
    if ne_puzzle_step == 2 then
      -- correct
      ne_puzzle_set_step(ne_puzzle_step + 1)
    else
      -- wrong
      ne_puzzle_set_step(1)
    end

  elseif sensor_name == "ne_puzzle_sensor_2" then

    sol.map.hero_set_position(2408, 397)
    if ne_puzzle_step == 1
      	or ne_puzzle_step == 3
	or ne_puzzle_step == 4 then
      -- correct
      ne_puzzle_set_step(ne_puzzle_step + 1)
      if ne_puzzle_step == 5 then
	sol.main.play_sound("secret")
      end
    else
      -- wrong
      ne_puzzle_set_step(1)
    end

  -- door G
  elseif sensor_name == "door_g_success_sensor"
      and not door_g_finished then
    sol.main.play_sound("secret")
    door_g_finished = true

  elseif sensor_name:find("^close_door_g_sensor")
      and not door_g_finished
      and sol.map.door_is_open("door_g") then
    sol.main.play_sound("wrong")
    sol.map.camera_move(1760, 520, 1000, function()
      sol.map.door_close("door_g")
      sol.map.switch_set_activated("door_g_switch", false)
    end) 

  -- door E
  elseif sensor_name:find("^close_door_e_sensor")
      and sol.map.door_is_open("door_e") then
    sol.map.door_close("door_e")
    sol.map.switch_set_activated("door_e_switch", false)

  -- west enemies room
  elseif sensor_name:find("^close_door_b_sensor")
      and not sol.map.enemy_is_group_dead("door_b_enemy")
      and sol.map.door_is_open("door_b") then
    sol.map.door_close("door_b")
    sol.map.sensor_set_group_enabled("close_door_b_sensor", false)

  -- north enemies room
  elseif sensor_name:find("^close_door_c_sensor")
      and not sol.map.enemy_is_group_dead("door_c_enemy")
      and sol.map.door_is_open("door_c") then
    sol.map.door_close("door_c")
    sol.map.sensor_set_group_enabled("close_door_c_sensor", false)

  -- reset solid ground location
  elseif sensor_name:find("^reset_solid_ground_sensor") then
    sol.map.hero_reset_solid_ground()
  end
end

function event_enemy_dead(enemy_name)

  -- west enemies room
  if enemy_name:find("^door_b_enemy") then
    if sol.map.enemy_is_group_dead("door_b_enemy")
        and not sol.map.door_is_open("door_b") then
      sol.main.play_sound("secret")
      sol.map.door_open("door_b")
    end

  -- north enemies room
  elseif enemy_name:find("^door_c_enemy") then
    if sol.map.enemy_is_group_dead("door_c_enemy")
        and not sol.map.door_is_open("door_c") then
      sol.main.play_sound("secret")
      sol.map.door_open("door_c")
    end

  end
end

function ne_puzzle_set_step(step)

  ne_puzzle_step = step
  sol.map.tile_set_group_enabled("ne_puzzle_step", false)
  sol.map.tile_set_group_enabled("ne_puzzle_step_" .. step, true)
  if step < 5 then
    sol.map.tile_set_group_enabled("secret_way_open", false)
    sol.map.tile_set_group_enabled("secret_way_closed", true)
  else
    sol.map.tile_set_group_enabled("secret_way_open", true)
    sol.map.tile_set_group_enabled("secret_way_closed", false)
  end
end

function event_chest_empty(chest_name)

  local index = tonumber(chest_name:match("^compass_chest_([1-7])"))
  if index ~= nil then
    if index == chests_puzzle_step then
      if index == 7 then
	sol.map.treasure_give("compass", 1, 814)
      else
        sol.map.hero_unfreeze()
	chests_puzzle_step = chests_puzzle_step + 1
      end
    else
      sol.main.play_sound("wrong")
      sol.map.hero_unfreeze()
      chests_puzzle_step = 1
      for i = 1, 7 do
	sol.map.chest_set_open("compass_chest_" .. i, false)
      end
    end
  end
end

-- Torches on this map interact with the map script
-- because we don't want usual behavior from items/lamp.lua:
-- we want a shorter delay and we want torches to enable the bridge
function event_npc_interaction(npc_name)

  if string.find(npc_name, "^torch") then
    sol.map.dialog_start("torch.need_lamp")
  end
end

-- Called when fire touches an NPC linked to this map
function event_npc_collision_fire(npc_name)

  if string.find(npc_name, "^torch") then
    
    local torch_sprite = sol.map.npc_get_sprite(npc_name)
    if sol.main.sprite_get_animation(torch_sprite) == "unlit" then
      -- temporarily light the torch up
      sol.main.sprite_set_animation(torch_sprite, "lit")
      if nb_torches_lit == 0 then
        sol.map.tile_set_group_enabled("bridge", true)
      end
      nb_torches_lit = nb_torches_lit + 1
      sol.main.timer_start(function()
        sol.main.sprite_set_animation(torch_sprite, "unlit")
        nb_torches_lit = nb_torches_lit - 1
        if nb_torches_lit == 0 then
	  sol.map.tile_set_group_enabled("bridge", false)
	end
      end, 8000)
    end
  end
end

function event_block_moved(block_name)

  if block_name == "door_f_block"
      and not sol.map.door_is_open("door_f") then
    sol.main.play_sound("secret")
    sol.map.door_open("door_f")
  end
end

