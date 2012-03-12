-- Dungeon 9 1F

-- puzzle A
local puzzle_a_switches = sol.map.find("puzzle_a_switch")
local puzzle_a_red_tiles = sol.map.find("puzzle_a_red_tile")
local puzzle_a_green_tiles = sol.map.find("puzzle_a_green_tile")

-- puzzle B
local puzzle_b_switches = sol.map.find("puzzle_b_switch")
local puzzle_b_next = nil -- index of the next correct switch (nil = error or not started)
local puzzle_b_nb_activated = 0

-- torches and bridges
local torches = sol.map.find("torch")
local nb_torches_lit = 0
local bridge_tiles = sol.map.find("bridge_tile")

-- hidden enemies
local hidden_enemies = sol.map.find("hidden_enemy")

function sol.map.on_started(destination)

  -- hidden Gibdo and chest
  hidden_enemy_1:set_enabled(false)
  hidden_enemy_2:set_enabled(false)
  if not sol.game:get_boolean(800) then
    hidden_enemy_chest:set_enabled(false)
  end

  if sol.game:get_boolean(802) then
    -- already solved
    puzzle_a_red_tiles:set_enabled(false)
    puzzle_a_switches:set_enabled(false)
  else
    -- not solved yet
    puzzle_a_chest:set_enabled(false)
  end
  puzzle_a_green_tiles:set_enabled(false)

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
  bridge_tiles:set_enabled(false)

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
      and hidden_enemy_1 ~= nil
      and hidden_enemy_1:is_enabled()
      and hidden_enemy_2 ~= nil
      and hidden_enemy_2:is_enabled() then

    sol.audio.play_sound("cane")
    hidden_enemies:set_enabled()
  end
end

-- hidden enemies
function hidden_enemies:on_killed()

  if #sol.map.find("hidden_enemy") == 0
      and not hidden_enemy_chest:is_enabled() then
    sol.map.camera_move(1128, 2040, 250, function()
      sol.audio.play_sound("chest_appears")
      hidden_enemy_chest:set_enabled()
    end)
  end
end

-- south door
function sol.map.find("s_door_enemy"):on_killed()

  if #sol.map.find("s_door_enemy") == 0
      and not s_door:is_open() then
    sol.map.camera_move(1768, 1800, 250, function()
      sol.audio.play_sound("secret")
      s_door:open()
    end)
  end
end

-- east enemies room
function sol.map.find("e_room_enemy"):on_killed()

  if #sol.map.find("e_room_enemy") == 0
      and not e_room_chest:is_enabled() then
    sol.map.camera_move(2136, 1120, 250, function()
      sol.audio.play_sound("chest_appears")
      e_room_chest:set_enabled()
    end)
  end
end

-- door to puzzle B
function puzzle_b_door_switch:on_activated()

  if not puzzle_b_door:is_open() then
    sol.map.camera_move(808, 1544, 250, function()
      sol.audio.play_sound("secret")
      puzzle_b_door:open()
      puzzle_b_door_switch:set_activated()
    end)
  end
end

-- north-west chest
function sol.map.find("nw_switch"):on_activated()

  if not nw_chest:is_enabled()
      and nw_switch_1:is_activated()
      and nw_switch_2:is_activated() then
    sol.audio.play_sound("chest_appears")
    nw_chest:set_enabled(true)
  end
end

-- central room
function sol.map.find("c_room_switch"):on_activated()

  if sol.map.find("c_room_switch"):is_activated() then
    sol.audio.play_sound("secret")
    c_door:open()
  end
end

-- puzzle B: the switches have to be activated clockwise
function puzzle_b_switches:on_activated()

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
      sol.audio.play_sound("wrong")
      puzzle_b_switches:set_activated(false)
      puzzle_b_nb_activated = 0
      self:set_locked()
      -- to avoid the switch to be activated again immediately
    else
      -- correct
      sol.audio.play_sound("secret")
      puzzle_b_door:open()
      puzzle_b_door_switch:set_activated()
    end
  end
end

-- puzzle A: each switch changes its neighboors in the 4 main directions
function puzzle_a_switches:on_activated()

  sol.audio.play_sound("switch")
  local to_change = { {2,4}, {1,3,5}, {2,6}, {1,5}, {2,4,6}, {3,5} }
  local index = tonumber(self:get_name():match("^puzzle_a_switch_([1-6])$"))
  if index then

    -- invert the neighboors
    for _, v in ipairs(to_change[index]) do
      local on = sol.map.entity("puzzle_a_switch_red_" .. v):is_enabled()
      sol.map.entity("puzzle_a_switch_green_" .. v):set_enabled(on)
      sol.map.entity("puzzle_a_switch_red_" .. v):set_enabled(not on)
    end

    -- check the success
    local success = true
    for i = 1, 6 do
      if sol.map.entity("puzzle_a_switch_red_" .. i):is_enabled() then
        success = false
        break
      end
    end
    if success then
      sol.map.find("puzzle_a_switch"):set_enabled(false)
      sol.map.camera_move(896, 1896, 250, function()
        sol.audio.play_sound("chest_appears")
        puzzle_a_chest:set_enabled()
        sol.game:set_boolean(802, true)
      end)
    end
  end
end

function puzzle_b_switches:on_left()
  self:set_locked(false)
end

function close_w_room_sensor.on_collision_hero()

  -- west room
  if w_room_door:is_open()
      and not w_room_chest:is_enabled() then
    w_room_door:close()

    function w_room_enemy_on_killed()
      if #sol.map.find("w_room_enemy") == 0 then
	sol.audio.play_sound("chest_appears")
	w_room_chest:set_enabled()
	w_room_door:open()
      end
    end

    sol.map.create_enemy("w_room_enemy_1",
      { x = 752, y = 877, layer = 1, breed = "blue_pig_soldier",
        on_killed = w_room_enemy_on_killed })
    sol.map.create_enemy("w_room_enemy_2",
      { x = 808, y = 885, layer = 1, breed = "red_pig_soldier",
        on_killed = w_room_enemy_on_killed })
    sol.map.create_enemy("w_room_enemy_3",
      { x = 864, y = 877, layer = 1, breed = "blue_pig_soldier",
        on_killed = w_room_enemy_on_killed })
  end
end

function open_w_room_sensor:on_collision_hero()
  w_room_door:set_open()
end

function sol.map.find("close_c_doors_sensor"):on_collision_hero()

  if c_door_e:is_open()
      and not c_room_switch_1:is_activated() then
    if c_big_key_door:is_open() then
      c_door_s:close()
    end
    c_door_e:close()
    c_room_switch_1:set_activated(false)
    c_room_switch_2:set_activated(false)
    c_room_switch_3:set_activated(false)
    c_room_switch_4:set_activated(false)
  end
end
  
-- puzzle B
function sol.map.find("close_puzzle_b_door_sensor"):on_collision_hero()

  if not puzzle_b_switch_1:is_activated()
      and puzzle_b_door:is_open() then
    puzzle_b_door:close()
    puzzle_b_door_switch:set_activated(false)
  end
end

-- reset solid ground location
function sol.map.find("reset_solid_ground_sensor"):on_collision_hero()
  hero:reset_solid_ground()
end

-- Torches on this map interact with the map script
-- because we don't want usual behavior from items/lamp.lua:
-- we want a shorter delay and we want torches to enable the bridge
function torches:on_interaction()
  sol.map.dialog_start("torch.need_lamp")
end

-- Called when fire touches an NPC linked to this map
function torches:on_collision_fire()

  local torch_sprite = self:get_sprite()
  if torch_sprite:get_animation() == "unlit" then
    -- temporarily light the torch up
    torch_sprite:set_animation("lit")
    if nb_torches_lit == 0 then
      bridge_tiles:set_enabled()
    end
    nb_torches_lit = nb_torches_lit + 1
    sol.timer.create(4000, function()
      torch_sprite:set_animation("unlit")
      nb_torches_lit = nb_torches_lit - 1
      if nb_torches_lit == 0 then
	bridge_tiles:set_enabled(false)
      end
    end)
  end
end

