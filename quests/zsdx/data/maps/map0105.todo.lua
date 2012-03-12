-- Dungeon 9 1F

-- puzzle B
local puzzle_b_next = nil -- index of the next correct switch (nil = error or not started)
local puzzle_b_nb_activated = 0

-- torches and bridges
local nb_torches_lit = 0

function sol.map.on_started(destination)

  -- hidden Gibdo and chest
  hidden_enemy_1:set_enabled(false)
  hidden_enemy_2:set_enabled(false)
  if not sol.game:get_boolean(800) then
    hidden_enemy_chest:set_enabled(false)
  end

  -- puzzle A
  if sol.game:get_boolean(802) then
    -- already solved
    sol.map.all("puzzle_a_switch_red"):set_enabled(false)
    sol.map.all("puzzle_a_switch"):set_enabled(false)
  else
    -- not solved yet
    puzzle_a_chest:set_enabled(false)
  end
  sol.map.all("puzzle_a_switch_green"):set_enabled(false)

  -- puzzle B
  if destination == from_b1_w
      or destination == from_b1_e then
    puzzle_b_door:set_open()
    puzzle_b_door_switch:set_activated()
  end

  -- south door
  if destination ~= from_outside then
    s_door:set_open()
  end

  -- bridges that appear when a torch is lit
  sol.map.all("bridge"):set_enabled(false)

  -- west enemies room
  if not sol.game:get_boolean(806) then
    w_room_chest:set_enabled(false)
  else
    w_room_door:set_open(true)
  end

  -- central room
  c_door_e:set_open(true)
  c_door_s:set_open(true)

  -- east enemies room
  if not sol.game:get_boolean(808) then
    e_room_chest:set_enabled(false)
  end

  -- north-west chest
  if not sol.game:get_boolean(810) then
    nw_chest:set_enabled(false)
  else
    nw_switch_1:set_activated(true)
    nw_switch_2:set_activated(true)
  end

  -- shortcut to the boss
  if not sol.game:get_boolean(816) then
    shortcut:set_enabled(false)
    shortcut_teletransporter:set_enabled(false)
  end
end

function sol.map.on_opening_transition_finished(destination)

  -- show the welcome message
  if destination == from_outside then
    sol.map.dialog_start("dungeon_9.welcome")
  end
end

function sol.map.on_update()

  -- hidden enemies
  local x, y = hero:get_position()
  if x > 1056 and x < 1200 and y > 1888 and y < 1968
      and not hidden_enemy_chest:is_enabled()
      and not hidden_enemy_1:is_dead()
      and not hidden_enemy_2:is_dead()
      and not hidden_enemy_1:is_enabled()
      and not hidden_enemy_2:is_enabled() then

    sol.main.play_sound("cane")
    hidden_enemy_1:set_enabled()
    hidden_enemy_2:set_enabled()
  end
end

function hidden_enemy_1:on_dead()
  -- TODO remove (just an example)
end

sol.map.all("hidden_enemy"):on_dead = function()
  -- TODO remove (just an example)
end

-- hidden enemies
function sol.map.all("hidden_enemy"):on_dead()

  if sol.map.all("hidden_enemy"):is_dead()
      and not hidden_enemy_chest:is_enabled() then
    sol.map.camera_move(1128, 2040, 250, function()
      sol.main.play_sound("chest_appears")
      hidden_enemy_chest:set_enabled()
    end)
  end
end

-- south door
function sol.map.all("s_door_enemy"):on_dead()

  if sol.map.all("s_door_enemy"):is_dead()
      and not s_door:is_open() then
    sol.map.camera_move(1768, 1800, 250, function()
      sol.main.play_sound("secret")
      s_door:open()
    end)
  end
end

-- west enemies room
function sol.map.all("w_room_enemy"):on_dead()
    
  if sol.map.all("w_room_enemy"):is_dead() then
    sol.main.play_sound("chest_appears")
    w_room_chest:set_enabled()
    w_room_door:open()
  end
end

-- east enemies room
function sol.map.all("e_room_enemy"):on_dead()

  if sol.map.all("e_room_enemy"):is_dead()
      and not e_room_chest:is_enabled() then
    sol.map.camera_move(2136, 1120, 250, function()
      sol.main.play_sound("chest_appears")
      e_room_chest:set_enabled()
    end)
  end
end

-- door to puzzle B
function puzzle_b_door_switch:on_activated()

  if not puzzle_b_door:is_open() then
    sol.map.camera_move(808, 1544, 250, function()
      sol.main.play_sound("secret")
      puzzle_b_door:open()
      puzzle_b_door_switch:set_activated()
    end)
  end
end

-- north-west chest
function sol.map.all("nw_switch"):on_activated()

  if not nw_chest:is_enabled()
      and nw_switch_1:is_activated()
      and nw_switch_2:is_activated() then
    sol.main.play_sound("chest_appears")
    nw_chest:set_enabled(true)
  end
end

-- central room
function sol.map.all("c_room_switch"):on_activated()

  if sol.map.all("c_room_switch"):is_activated() then
    sol.main.play_sound("secret")
    c_door:open()
  end

  -- puzzle B: the switches have to be activated clockwise
function sol.map.all("puzzle_b_switch"):on_activated()

  local index = tonumber(self:get_name():match("^puzzle_b_switch_([1-4])$"))
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
      sol.main.play_sound("wrong")
      sol.map.all("puzzle_b_switch_"):set_activated(false)
      puzzle_b_nb_activated = 0
      self:set_locked()
      -- to avoid the switch to be activated again immediately
    else
      -- correct
      sol.main.play_sound("secret")
      puzzle_b_door:open()
      puzzle_b_door_switch:set_activated()
    end
  end
end

-- puzzle A: each switch changes its neighboors in the 4 main directions
function sol.map.all("puzzle_a_switch"):on_activated()

  sol.main.play_sound("switch")
  local to_change = { {2,4}, {1,3,5}, {2,6}, {1,5}, {2,4,6}, {3,5} }
  local index = tonumber(self:get_name():match("^puzzle_a_switch_([1-6])$"))
  if index then

    -- invert the neighboors
    for i, v in ipairs(to_change[index]) do
      local on = sol.map.tile_is_enabled("puzzle_a_switch_red_" .. v)
      sol.map.tile_set_enabled("puzzle_a_switch_green_" .. v, on)
      sol.map.tile_set_enabled("puzzle_a_switch_red_" .. v, not on)
    end

    -- check the success
    local success = true
    for i = 1, 6 do
      if sol.map.tile_is_enabled("puzzle_a_switch_red_" .. i) then
        success = false
        break
      end
    end
    if success then
      sol.map.tile_set_group_enabled("puzzle_a_switch", false)
      sol.map.switch_set_group_enabled("puzzle_a_switch", false)
      sol.map.camera_move(896, 1896, 250, function()
        sol.main.play_sound("chest_appears")
        sol.map.chest_set_enabled("puzzle_a_chest", true)
        sol.game:set_boolean(802, true)
      end)
    end
  end
end

function event_switch_left(switch_name)

  if switch_name:find("^puzzle_b_switch") then
    sol.map.switch_set_locked(switch_name, false)
  end
end

function event_hero_on_sensor(sensor_name)

  -- west room
  if sensor_name == "close_w_room_sensor" then

    if sol.map.door_is_open("w_room_door")
	 and not sol.map.chest_is_enabled("w_room_chest") then
      sol.map.door_close("w_room_door")
      sol.map.enemy_create("w_room_enemy_1", "blue_pig_soldier", 1, 752, 877)
      sol.map.enemy_create("w_room_enemy_2", "red_pig_soldier", 1, 808, 885)
      sol.map.enemy_create("w_room_enemy_3", "blue_pig_soldier", 1, 864, 877)
    end
  elseif sensor_name == "open_w_room_sensor" then
    sol.map.door_set_open("w_room_door", true)

  -- central room
  elseif sensor_name:find("^close_c_doors_sensor")
      and sol.map.door_is_open("c_door_e")
      and not sol.map.switch_is_activated("c_room_switch_1") then
    if sol.map.door_is_open("c_big_key_door") then
      sol.map.door_close("c_door_s")
    end
    sol.map.door_close("c_door_e")
    sol.map.switch_set_activated("c_room_switch_1", false)
    sol.map.switch_set_activated("c_room_switch_2", false)
    sol.map.switch_set_activated("c_room_switch_3", false)
    sol.map.switch_set_activated("c_room_switch_4", false)
  
  -- puzzle B
  elseif sensor_name:find("^close_puzzle_b_door_sensor") then

    if not sol.map.switch_is_activated("puzzle_b_switch_1")
        and sol.map.door_is_open("puzzle_b_door") then
      sol.map.door_close("puzzle_b_door")
      sol.map.switch_set_activated("puzzle_b_door_switch", false)
    end

  -- reset solid ground location
  elseif sensor_name:find("^reset_solid_ground_sensor") then
    sol.map.hero_reset_solid_ground()
  end
end

-- Torches on this map interact with the map script
-- because we don't want usual behavior from items/lamp.lua:
-- we want a shorter delay and we want torches to enable the bridge
function event_npc_interaction(npc_name)

  if npc_name:find("^torch") then
    sol.map.dialog_start("torch.need_lamp")
  end
end

-- Called when fire touches an NPC linked to this map
function event_npc_collision_fire(npc_name)

  if string.find(npc_name, "^torch") then
    
    local torch_sprite = sol.map.npc_get_sprite(npc_name)
    if torch_sprite:get_animation() == "unlit" then
      -- temporarily light the torch up
      torch_sprite:set_animation("lit")
      if nb_torches_lit == 0 then
        sol.map.tile_set_group_enabled("bridge", true)
      end
      nb_torches_lit = nb_torches_lit + 1
      sol.main.timer_start(function()
        torch_sprite:set_animation("unlit")
        nb_torches_lit = nb_torches_lit - 1
        if nb_torches_lit == 0 then
	  sol.map.tile_set_group_enabled("bridge", false)
	end
      end, 4000)
    end
  end
end

