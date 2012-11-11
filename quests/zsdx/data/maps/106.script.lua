local map = ...
-- Dungeon 9 2F

local ne_puzzle_step = nil
local chests_puzzle_step = nil
local nw_switches_next = nil -- index of the next correct switch (nil = error or not started)
local nw_switches_nb_activated = 0
local nb_torches_lit = 0
local door_g_finished = false

local function ne_puzzle_set_step(step)

  ne_puzzle_step = step
  map:set_entities_enabled("ne_puzzle_step", false)
  map:set_entities_enabled("ne_puzzle_step_" .. step, true)
  if step < 5 then
    map:set_entities_enabled("secret_way_open", false)
    map:set_entities_enabled("secret_way_closed", true)
  else
    map:set_entities_enabled("secret_way_open", true)
    map:set_entities_enabled("secret_way_closed", false)
  end
end

function map:on_started(destination)

  -- north barrier
  if map:get_game():get_value("b812") then
    n_barrier_switch:set_activated(true)
    n_barrier:set_enabled(false)
  end

  -- enemies rooms
  map:set_doors_open("door_c", true)
  if destination:get_name() ~= "from_3f_e"
      and destination ~= "from_outside_e" then
    map:set_doors_open("door_b", true)
  end

  -- north-east room
  if destination:get_name() == "from_3f_e" then
    map:set_doors_open("door_a", true)
    ne_puzzle_set_step(5)
  else
    ne_puzzle_set_step(1)
  end

  -- compass
  if map:get_game():get_value("b814") then
    for i = 1, 7 do
      map:get_entity("compass_chest_" .. i):set_open(true)
    end
  else
    chests_puzzle_step = 1
  end

  -- clockwise switches and next doors
  if destination:get_name() ~= "from_1f" then
    map:set_doors_open("door_d", true)
    map:set_doors_open("door_e", true)
    door_e_switch:set_activated(true)
    for i = 1, 8 do
      map:get_entity("nw_switch_" .. i):set_activated(true)
    end
  end

  -- bridges that appear when a torch is lit
  map:set_entities_enabled("bridge", false)
end

function map:on_opening_transition_finished(destination)

  -- show the welcome message
  if destination:get_name():find("^from_outside") then
    map:start_dialog("dungeon_9.welcome")
  end
end

-- north barrier
function n_barrier_switch:on_activated()

  sol.audio.play_sound("secret")
  sol.audio.play_sound("door_open")
  n_barrier:set_enabled(false)
  map:get_game():set_value("b812", true)
end

-- door A
function door_a_switch:on_activated()

  sol.audio.play_sound("secret")
  map:open_doors("door_a")
end

-- door E
function door_e_switch:on_activated()

  sol.audio.play_sound("secret")
  map:open_doors("door_e")
end

-- door G
function door_g_switch:on_activated()

  if not door_g:is_open() then
    map:move_camera(1760, 520, 1000, function()
      sol.audio.play_sound("secret")
      map:open_doors("door_g")
      door_g_finished = false
    end)
  end
end

-- north-west puzzle: the switches have to be activated clockwise
local function nw_switch_activated(switch)

  local index = tonumber(switch:get_name():match("^nw_switch_([1-8])$"))
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
      sol.audio.play_sound("wrong")
      for i = 1, 8 do
        map:get_entity("nw_switch_" .. i):set_activated(false)
      end
      nw_switches_nb_activated = 0
      switch:set_locked(true)
      -- to avoid the switch to be activated again immediately
    else
      -- correct
      sol.audio.play_sound("secret")
      map:open_doors("door_d")
    end
  end
end

local function nw_switch_left(switch)

  self:set_locked(false)
end

for _, switch in ipairs(map:get_entities("nw_switch")) do
  switch.on_activated = nw_switch_activated
  switch.on_left = nw_switch_left
end

-- north-east puzzle
function ne_puzzle_sensor_1:on_activated()

  hero:set_position(2408, 653)
  if ne_puzzle_step == 2 then
    -- correct
    ne_puzzle_set_step(ne_puzzle_step + 1)
  else
    -- wrong
    ne_puzzle_set_step(1)
  end
end

function ne_puzzle_sensor_2:on_activated()

  hero:set_position(2408, 397)
  if ne_puzzle_step == 1
      or ne_puzzle_step == 3
      or ne_puzzle_step == 4 then
    -- correct
    ne_puzzle_set_step(ne_puzzle_step + 1)
    if ne_puzzle_step == 5 then
      sol.audio.play_sound("secret")
    end
  else
    -- wrong
    ne_puzzle_set_step(1)
  end
end

-- door G
function door_g_success_sensor:on_activated()

  if not door_g_finished then
    sol.audio.play_sound("secret")
    door_g_finished = true
  end
end

local function close_door_g()

  if not door_g_finished
      and door_g:is_open() then
    sol.audio.play_sound("wrong")
    map:move_camera(1760, 520, 1000, function()
      map:close_doors("door_g")
      door_g_switch:set_activated(false)
    end)
  end
end

for _, sensor in ipairs(map:get_entities("close_door_g_sensor") do
  sensor.on_activated = close_door_g
end

-- door E
local function close_door_e()

  if door_e:is_open() then
    map:close_doors("door_e")
    door_e_switch:set_activated(false)
  end
end

for _, sensor in ipairs(map:get_entities("close_door_e_sensor") do
  sensor.on_activated = close_door_e
end

-- west enemies room
function close_door_b_sensor:on_activated()

  if map:has_entities("door_b_enemy")
      and door_b:is_open() then
    map:close_doors("door_b")
    close_door_b_sensor:set_enabled(false)
    close_door_b_sensor_2:set_enabled(false)
  end
end
close_door_b_sensor_2.on_activated = close_door_b_sensor_.on_activated 


-- north enemies room
function close_door_c_sensor:on_activated()

  if map:has_entities("door_c_enemy")
      and door_c:is_open() then
    map:close_doors("door_c")
    close_door_c_sensor:set_enabled(false)
    close_door_c_sensor_2:set_enabled(false)
  end
end
close_door_c_sensor_2.on_activated = close_door_c_sensor_.on_activated 
--
-- save solid ground location
for _, sensor in ipairs(map:get_entities("save_solid_ground_sensor")) do
  function sensor:on_activated()
    hero:save_solid_ground()
  end
end

-- reset solid ground location
for _, sensor in ipairs(map:get_entities("reset_solid_ground_sensor")) do
  function sensor:on_activated()
    hero:reset_solid_ground()
  end
end

-- west enemies room
local function door_b_enemy_dead(enemy)

  if not map:has_entities("door_b_enemy")
      and not door_b:is_open() then
    sol.audio.play_sound("secret")
    map:open_doors("door_b")
  end
end

for _, enemy in ipairs(map:get_entities("door_b_enemy")) do
  enemy.on_dead = door_b_enemy_dead
end

-- north enemies room
local function door_c_enemy_dead(enemy)

  if not map:has_entities("door_c_enemy")
      and not door_c:is_open() then
    sol.audio.play_sound("secret")
    map:open_doors("door_c")
  end
end

for _, enemy in ipairs(map:get_entities("door_c_enemy")) do
  enemy.on_dead = door_c_enemy_dead
end

local function compass_chest_empty(chest)

  local index = tonumber(chest:get_name():match("^compass_chest_([1-7])"))
  if index ~= nil then
    if index == chests_puzzle_step then
      if index == 7 then
	hero:start_treasure("compass", 1, 814)
      else
        hero:unfreeze()
	chests_puzzle_step = chests_puzzle_step + 1
      end
    else
      sol.audio.play_sound("wrong")
      hero:unfreeze()
      chests_puzzle_step = 1
      for i = 1, 7 do
	map:get_entity("compass_chest_" .. i):set_open(false)
      end
    end
  end
end

for _, chest in ipairs(map:get_entities("compass_chest_")) do
  chest.on_empty = compass_chest_empty
end

-- Torches on this map interact with the map script
-- because we don't want usual behavior from items/lamp.lua:
-- we want a shorter delay and we want torches to enable the bridge
local function torch_interaction(torch)
  map:start_dialog("torch.need_lamp")
end

-- Called when fire touches a torch.
local function torch_collsion_fire(torch)

  local torch_sprite = torch:get_sprite()
  if torch_sprite:get_animation() == "unlit" then
    -- temporarily light the torch up
    torch_sprite:set_animation("lit")
    if nb_torches_lit == 0 then
      map:set_entities_enabled("bridge", true)
    end
    nb_torches_lit = nb_torches_lit + 1
    sol.timer.start(4000, function()
      torch_sprite:set_animation("unlit")
      nb_torches_lit = nb_torches_lit - 1
      if nb_torches_lit == 0 then
	map:set_entities_enabled("bridge", false)
      end
    end)
  end
end

for _, torch in ipairs(map:get_entities("torch_")) do
  torch.on_interaction = torch_interaction
  torch.on_collision_fire = torch_collision_fire
end

function door_f_block:on_moved()

  if not door_f:is_open() then
    sol.audio.play_sound("secret")
    map:open_doors("door_f")
  end
end

