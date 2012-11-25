local map = ...
-- Dungeon 9 1F

-- puzzle B
local puzzle_b_next = nil -- index of the next correct switch (nil = error or not started)
local puzzle_b_nb_activated = 0

-- torches and bridges
local nb_torches_lit = 0

function map:on_started(destination)

  -- hidden Gibdo and chest
  map:set_entities_enabled("hidden_enemy", false)
  if not map:get_game():get_value("b800") then
    hidden_chest:set_enabled(false)
  end

  -- puzzle A
  if map:get_game():get_value("b802") then
    -- already solved
    map:set_entities_enabled("puzzle_a_red", false)
    map:set_entities_enabled("puzzle_a_switch", false)
  else
    -- not solved yet
    puzzle_a_chest:set_enabled(false)
  end
  map:set_entities_enabled("puzzle_a_green", false)

  -- puzzle B
  if destination:get_name() == "from_b1_w"
      or destination:get_name() == "from_b1_e" then
    map:set_doors_open("puzzle_b_door", true)
    puzzle_b_door_switch:set_activated(true)
  end

  -- south door
  if destination:get_name() ~= "from_outside" then
    map:set_doors_open("s_door", true)
  end

  -- bridges that appear when a torch is lit
  map:set_entities_enabled("bridge", false)

  -- west enemies room
  if not map:get_game():get_value("b806") then
    w_room_chest:set_enabled(false)
  else
    map:set_doors_open("w_room_door", true)
  end

  -- central room
  map:set_doors_open("c_door_e", true)
  map:set_doors_open("c_door_s", true)

  -- east enemies room
  if not map:get_game():get_value("b808") then
    e_room_chest:set_enabled(false)
  end

  -- north-west chest
  if not map:get_game():get_value("b810") then
    nw_chest:set_enabled(false)
  else
    nw_switch_1:set_activated(true)
    nw_switch_2:set_activated(true)
  end

  -- shortcut to the boss
  if not map:get_game():get_value("b816") then
    shortcut:set_enabled(false)
    shortcut_teletransporter:set_enabled(false)
  end
end

function map:on_opening_transition_finished(destination)

  -- show the welcome message
  if destination:get_name() == "from_outside" then
    map:start_dialog("dungeon_9.welcome")
  end
end

function map:on_update()

  -- hidden enemies
  local x, y = hero:get_position()
  if x > 1056 and x < 1200 and y > 1888 and y < 1968
      and not hidden_chest:is_enabled()
      and hidden_enemy_1 ~= nil
      and hidden_enemy_2 ~= nil
      and not hidden_enemy_1:is_enabled()
      and not hidden_enemy_2:is_enabled() then

    sol.audio.play_sound("cane")
    hidden_enemy_1:set_enabled(true)
    hidden_enemy_2:set_enabled(true)
  end
end

-- hidden enemies
local function hidden_enemy_dead(enemy)

  if not map:has_entities("hidden_enemy")
      and not hidden_chest:is_enabled() then
    map:move_camera(1128, 2040, 250, function()
      sol.audio.play_sound("chest_appears")
      hidden_chest:set_enabled(true)
    end)
  end
end
for _, enemy in ipairs(map:get_entities("hidden_enemy")) do
  enemy.on_dead = hidden_enemy_dead
end

-- south door
local function s_door_enemy_dead(enemy)

  if not map:has_entities("s_door_enemy")
      and not s_door:is_open() then
    map:move_camera(1768, 1800, 250, function()
      sol.audio.play_sound("secret")
      map:open_doors("s_door")
    end)
  end
end
for _, enemy in ipairs(map:get_entities("s_door_enemy")) do
  enemy.on_dead = s_door_enemy_dead
end

-- west enemies room
local function w_room_enemy_dead(enemy)

  if not map:has_entities("w_room_enemy") then
    sol.audio.play_sound("chest_appears")
    w_room_chest:set_enabled(true)
    if not w_room_door:is_open() then
      map:open_doors("w_room_door")
    end
  end
end
for _, enemy in ipairs(map:get_entities("w_room_enemy")) do
  enemy.on_dead = w_room_enemy_dead
end

-- east enemies room
local function e_room_enemy_dead(enemy)

  if not map:has_entities("e_room_enemy")
      and not e_room_chest:is_enabled() then
    map:move_camera(2136, 1120, 250, function()
      sol.audio.play_sound("chest_appears")
      e_room_chest:set_enabled(true)
    end)
  end
end
for _, enemy in ipairs(map:get_entities("e_room_enemy")) do
  enemy.on_dead = e_room_enemy_dead
end

-- door to puzzle B
function puzzle_b_door_switch:on_activated()

  if not puzzle_b_door:is_open() then
    map:move_camera(808, 1544, 250, function()
      sol.audio.play_sound("secret")
      map:open_doors("puzzle_b_door")
      puzzle_b_door_switch:set_activated(true)
    end)
  end
end

-- north-west chest
function nw_switch_1:on_activated()

  if not nw_chest:is_enabled()
      and nw_switch_1:is_activated()
      and nw_switch_2:is_activated() then
    sol.audio.play_sound("chest_appears")
    nw_chest:set_enabled(true)
  end
end
nw_switch_2.on_activated = nw_switch_1.on_activated

-- central room
function c_room_switch_1:on_activated()

  if c_room_switch_1:is_activated()
      and c_room_switch_2:is_activated()
      and c_room_switch_3:is_activated()
      and c_room_switch_4:is_activated() then
    sol.audio.play_sound("secret")
    map:open_doors("c_door")
  end
end
c_room_switch_2.on_activated = c_room_switch_1.on_activated
c_room_switch_3.on_activated = c_room_switch_1.on_activated
c_room_switch_4.on_activated = c_room_switch_1.on_activated

-- puzzle B: the switches have to be activated clockwise
local function puzzle_b_switch_activated(switch)

  local index = tonumber(switch:get_name():match("^puzzle_b_switch_([1-4])$"))
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
	map:get_entity("puzzle_b_switch_" .. i):set_activated(false)
      end
      puzzle_b_nb_activated = 0
      switch:set_locked(true)
      -- to avoid the switch to be activated again immediately
    else
      -- correct
      sol.audio.play_sound("secret")
      map:open_doors("puzzle_b_door")
      puzzle_b_door_switch:set_activated(true)
    end
  end
end

local function puzzle_b_switch_left(switch)

  switch:set_locked(false)
end
for _, switch in ipairs(map:get_entities("puzzle_b_switch")) do
  switch.on_activated = puzzle_b_switch_activated
  switch.on_left = puzzle_b_switch_left
end

-- puzzle A: each switch changes its neighboors in the 4 main directions
local function puzzle_a_switch_activated(switch)

  sol.audio.play_sound("switch")
  local to_change = { {2,4}, {1,3,5}, {2,6}, {1,5}, {2,4,6}, {3,5} }
  local index = tonumber(switch:get_name():match("^puzzle_a_switch_([1-6])$"))
  if index then

    -- invert the neighboors
    for i, v in ipairs(to_change[index]) do
      local red_tile = map:get_entity("puzzle_a_red_" .. v)
      local green_tile = map:get_entity("puzzle_a_green_" .. v)
      local on = red_tile:is_enabled()
      green_tile:set_enabled(on)
      red_tile:set_enabled(not on)
    end

    -- check the success
    local success = true
    for i = 1, 6 do
      if map:get_entity("puzzle_a_red_" .. i):is_enabled() then
	success = false
	break
      end
    end
    if success then
      map:set_entities_enabled("puzzle_a_switch", false)
      map:set_entities_enabled("puzzle_a_green", false)
      map:set_entities_enabled("puzzle_a_red", false)
      map:move_camera(896, 1896, 250, function()
	sol.audio.play_sound("chest_appears")
	puzzle_a_chest:set_enabled(true)
	map:get_game():set_value("b802", true)
      end)
    end
  end
end
for _, switch in ipairs(map:get_entities("puzzle_a_switch")) do
  switch.on_activated = puzzle_a_switch_activated
end

-- west room
function close_w_room_sensor:on_activated()

  if w_room_door:is_open()
      and not w_room_chest:is_enabled() then
    map:close_doors("w_room_door")
    map:create_enemy{
      name = "w_room_enemy_1",
      breed = "blue_pig_soldier",
      layer = 1,
      x = 752,
      y = 877,
      treasure_name = "random"
    }
    map:create_enemy{
      name = "w_room_enemy_2",
      breed = "red_pig_soldier",
      layer = 1,
      x = 808,
      y = 885,
      treasure_name = "random"
    }
    map:create_enemy{
      name = "w_room_enemy_3",
      breed = "blue_pig_soldier",
      layer = 1,
      x = 864,
      y = 877,
      treasure_name = "random"
    }
  end
end

function open_w_room_sensor:on_activated()
  map:set_doors_open("w_room_door", true)
end

-- central room
function close_c_doors_sensor:on_activated()

  if c_door_e:is_open()
      and not c_room_switch_1:is_activated() then
    if c_big_key_door:is_open() then
      map:close_doors("c_door_s")
    end
    map:close_doors("c_door_e")
    c_room_switch_1:set_activated(false)
    c_room_switch_2:set_activated(false)
    c_room_switch_3:set_activated(false)
    c_room_switch_4:set_activated(false)
  end
end
close_c_doors_sensor_2.on_activated = close_c_doors_sensor.on_activated

-- puzzle B
function close_puzzle_b_door_sensor:on_activated()

  if not puzzle_b_switch_1:is_activated()
      and puzzle_b_door:is_open() then
    map:close_doors("puzzle_b_door")
    puzzle_b_door_switch:set_activated(false)
  end
end
close_puzzle_b_door_sensor_2.on_activated = close_puzzle_b_door_sensor.on_activated

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

