local map = ...
-- Dungeon 7 2F

local fighting_miniboss = false
local code_nb_activated = 0
local code_next_index = 1

-- Returns whether all torches are on
local function are_all_torches_on()

  return torch_1:exists()
      and torch_1:get_sprite():get_animation() == "lit"
      and torch_2:get_sprite():get_animation() == "lit"
      and torch_3:get_sprite():get_animation() == "lit"
      and torch_4:get_sprite():get_animation() == "lit"
end

-- Makes all torches on forever
local function lock_torches()
  torch_1:remove()
  torch_2:remove()
  torch_3:remove()
  torch_4:remove()
end

function map:on_started(destination)

  -- pipe maze
  map:set_entities_enabled("pipe_border", false)

  -- door of the pots and pikes
  map:set_doors_open("door_f", true)
  door_f_switch:set_activated(true)

  -- west enemies room
  map:set_doors_open("door_c", true)
  if map:get_game():get_boolean(616) then
    local x, y = w_room_enemy_4:get_position()
    w_room_enemy_4:set_position(x, y, 1)
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
    weak_floor:set_enabled(false)
    weak_floor_2:set_enabled(false)
    weak_floor_sensor:set_enabled(false)
  else
    weak_floor_teletransporter:set_enabled(false)
  end

  -- miniboss
  map:set_doors_open("miniboss_door", true)
  map:set_entities_enabled("miniboss_enemy", false)

  -- save the north-west door from 1F
  if destination:get_name() == "from_1f_ne" then
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

-- close door F
function close_door_f_sensor:on_activated()

  map:set_doors_open("door_f", false)
  door_f_switch:set_activated(false)
end

-- door C (west room)
function close_door_c_sensor:on_activated()

  if map:has_entities("w_room_enemy")
      and door_c:is_open() then
    map:close_doors("door_c")
  end
end
close_door_c_sensor_2.on_activated = close_door_c_sensor.on_activated 

-- miniboss
function start_miniboss_sensor:on_activated()

  if not map:get_game():get_boolean(620)
      and not fighting_miniboss then

    hero:freeze()
    map:close_doors("miniboss_door")
    fighting_miniboss = true
    sol.timer.start(1000, function()
      sol.audio.play_music("boss")
      map:set_entities_enabled("miniboss_enemy", true)
      hero:unfreeze()
    end)
  end
end

-- pipes
local function pipe_sensor_in_activated(sensor)

  local pipe = sensor:get_name():match("^pipe_in_([a-z])_sensor")
  if pipe ~= nil then
    -- entering a pipe
    map:set_entities_enabled("pipe_under_" .. pipe, false)
    map:set_entities_enabled("pipe_over_" .. pipe, true)
    map:set_entities_enabled("pipe_border_" .. pipe, true)
    hero:set_visible(true)
  end
end
for _, sensor in ipairs(map:get_entities("pipe_in_")) do
  sensor:on_activated = pipe_sensor_in_activated
end

local function pipe_sensor_out_activated(sensor)

  local pipe = string.match(sensor_name, "^pipe_out_([a-z])_sensor")
  if pipe ~= nil then
    -- leaving a pipe
    map:set_entities_enabled("pipe_under_" .. pipe, true)
    map:set_entities_enabled("pipe_over_" .. pipe, false)
    map:set_entities_enabled("pipe_border_" .. pipe, false)
  end
end
for _, sensor in ipairs(map:get_entities("pipe_out_")) do
  sensor:on_activated = pipe_sensor_out_activated
end

local function hide_hero_sensor_activated(sensor)

  -- hide the hero
  hero:set_visible(false)
end

for _, sensor in ipairs(map:get_entities("hide_hero_sensor")) do
  sensor:on_activated = hide_hero_sensor_activated
end

local function unhide_hero_sensor_activated(sensor)

  -- unhide the hero
  hero:set_visible(true)
end
for _, sensor in ipairs(map:get_entities("unhide_hero_sensor")) do
  sensor:on_activated = unhide_hero_sensor_activated
end

-- door F
function door_f_switch:on_activated()

  map:move_camera(1040, 760, 250, function()
    sol.audio.play_sound("secret")
    map:open_doors("door_f")
  end)
end

-- door D
function door_d_switch:on_activated()

  sol.audio.play_sound("secret")
  map:open_doors("door_d")
end

-- shortcut to the boss
function shortcut_switch:on_activated()

  map:set_entities_enabled("shortcut_on", true)
  map:set_entities_enabled("shortcut_off", false)
  map:get_game():set_boolean(628, true)
  sol.audio.play_sound("secret")
end

-- code
local function code_switch_activated(switch)

  local index = tonumber(string.match(switch:get_name(), "^code_switch_([1-8])$"))
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
	self:set_locked(true)
	sol.timer.start(500, function()
	  self:set_locked(false)
	end)
      end
    end
  end
end
for _, switch in ipairs(map:get_entities("code_switch_")) do
  switch.on_activated = code_switch_activated
end

function door_b:on_open()

  -- put the last enemy of the room on the hero's layer
  if w_room_enemy_4 ~= nil then
    local x, y = w_room_enemy_4:get_position()
    w_room_enemy_4:set_position(x, y, 1)
  end
end

-- west enemies room
local function w_room_enemy_dead(enemy)

  if not map:has_entities("w_room_enemy") then
    sol.audio.play_sound("secret")
    if not door_c:is_open() then
      map:open_doors("door_c")
    end
    if not door_a:is_open() then
      map:open_doors("door_a")
    end
  end
end
for _, enemy in ipairs(map:get_entitites("w_room_enemy")) do
  enemy.on_dead = w_room_enemy_dead
end

-- miniboss
local function miniboss_enemy_dead(enemy)

  if not map:has_entities("miniboss_enemy") then

    sol.audio.play_music("dark_world_dungeon")
    sol.audio.play_sound("secret")
    map:open_doors("miniboss_door")
    map:get_game():set_boolean(620, true)
  end
end
for _, enemy in ipairs(map:get_entitites("miniboss_enemy")) do
  enemy.on_dead = miniboss_enemy_dead
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

function weak_floor_sensor:on_collision_explosion()

  if weak_floor:is_enabled() then

    weak_floor:set_enabled(false)
    weak_floor_2:set_enabled(false)
    weak_floor_sensor:set_enabled(false)
    weak_floor_teletransporter:set_enabled(true)
    sol.audio.play_sound("secret")
    map:get_game():set_boolean(619, true)
  end
end

