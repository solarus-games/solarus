local map = ...
-- Dungeon 7 1F

local next_sign = 1
local directions = {
  2, 1, 0, 3, 0, 3, 2, 1, 0, 3, 2, 3, 0, 1,
  2, 1, 0, 3, 0, 3, 0, 1, 2, 3, 2, 1, 2
}
local door_a_allow_close = true
local door_b_allow_close = true

function map:on_started(destination)

  map:set_entities_enabled("pipe_over", false)
  map:set_entities_enabled("pipe_under", true)
  map:set_entities_enabled("pipe_border", false)

  if map:get_game():get_value("621") then
    ne_door_switch:set_activated(true)
    ne_door_switch:set_locked(true)
  end
end

function map:on_opening_transition_finished(destination)

  -- show the welcome message
  if destination:get_name() == "from_outside" then
    map:start_dialog("dungeon_7.welcome")
  end
end

  -- door A (south-west timed door)
function door_a_dont_close_sensor:on_activated()
  -- disable the timer that would close the timed door
  door_a_allow_close = false
end
door_a_dont_close_sensor_2.on_activated = door_a_dont_close_sensor.on_activated

function door_a_close_sensor:on_activated()

  if door_a:is_open() then
    door_a_allow_close = true
    if not door_a_switch:is_activated() then
      -- the timer has expired in the meantime
      map:close_doors("door_a")
    end
  end
end
door_a_close_sensor_2.on_activated = door_a_close_sensor.on_activated

-- door B (south-east timed door)
function door_b_dont_close_sensor:on_activated()

  -- disable the timer that would close the timed door
  door_b_allow_close = false
end
door_b_dont_close_sensor_2.on_activated = door_b_dont_close_sensor.on_activated

function door_b_close_sensor:on_activated()

  if door_b:is_open() then
    door_b_allow_close = true
    if not door_b_switch:is_activated() then
      -- the timer has expired in the meantime
      map:close_doors("door_b")
    end
  end
end
door_b_close_sensor_2.on_activated = door_b_close_sensor.on_activated

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

local function sign_interaction(sign)

  if not sw_door:is_open() then
    if sign:get_name() == "sign_" .. next_sign then

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
-- sign maze
for _, sign in ipairs(map:get_entities("sign_")) do
  sign:on_interaction = sign_interaction
end

-- door A
function door_a_switch:on_activated()

  if not door_a:is_open() then
    -- open the door and close it in a few seconds
    sol.audio.play_sound("secret")
    map:open_doors("door_a")
    door_a_allow_close = true
    local timer = sol.timer.start(10000, function()
      door_a_switch:set_activated(false)
      if door_a_allow_close then
	map:close_doors("door_a")
      end
    end)
    timer:set_with_sound(true)
  end
end

-- door B
function door_b_switch:on_activated()

  if not door_b:is_open() then
    -- open the door and close it in a few seconds
    sol.audio.play_sound("secret")
    map:open_doors("door_b")
    door_b_allow_close = true
    local timer = sol.timer.start(15000, function()
      door_b_switch:set_activated(false)
      if door_b_allow_close then
	map:close_doors("door_b")
      end
    end)
    timer:set_with_sound(true)
  end
end

-- north-west door
function ne_door_switch:on_activated()
  map:open_doors("ne_door")
end

function ne_door_switch:on_inactivated()
  map:close_doors("ne_door")
end

