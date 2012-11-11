local map = ...
-- Dungeon 5 2F

sol.main.do_file("maps/prison_guard")

local puzzle_next_sensor = 1

local function init_guard(guard, path)

  local m = sol.movement.create("path")
  m:set_path(path)
  m:set_speed(48)
  m:set_loop(true)
  m:set_ignore_obstacles(true)
  guard:start_movement(m)
  guard:get_sprite():set_animation("walking")
end

-- Returns whether all torches are on
local function are_all_torches_on()

  return torch_1 ~= nil
      and torch_1:get_sprite():get_animation() == "lit"
      and torch_2:get_sprite():get_animation() == "lit"
      and torch_3:get_sprite():get_animation() == "lit"
end

-- Makes all torches on forever
local function lock_torches()
  torch_1:remove()
  torch_2:remove()
  torch_3:remove()
end

local function puzzle_solved()

  hero:freeze()
  sol.audio.play_sound("enemy_awake")
  sol.timer.start(1000, function()
    sol.timer.start(1000, function()
      sol.audio.play_sound("secret")
      map:get_game():set_value("519", true)
      map:start_dialog("dungeon_5.puzzle_solved", function()
	hero:unfreeze()
      end)
    end)
  end)
end
function map:on_started(destination)

  init_guard(guard_3, {6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,4,4,4,4,4,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0})

  if map:get_game():get_value("515") then
    weak_floor:set_enabled(false)
    weak_floor_sensor:set_enabled(false)
    weak_floor_block:set_enabled(false)
  end

  -- blocks necessary to go back when found the feather
  if map:get_game():get_value("517") then
    s_block_1:set_position(464, 773)
    s_block_2:set_position(496, 773)
  end
end

function map:on_opening_transition_finished(destination)

  -- show the welcome message
  if destination:get_name() == "from_roof"
      or destination:get_name() == "from_outside_w"
      or destination:get_name() == "from_outside_e" then
    map:start_dialog("dungeon_5.welcome")
  end
end

function puzzle_wrong_sensor:on_activated()

  puzzle_next_sensor = 1
end

function save_s_blocks_sensor:on_activated()

  if map:get_game():get_item("feather"):has_variant()
      and not map:get_game():get_value("517") then
    -- solved the blocks puzzle necessary to exit this floor
    map:get_game():set_value("517", true)
  end
end

local function puzzle_sensor_activated(sensor)
  local i = sensor:get_name():match("puzzle_sensor_([1-4])")
  if i ~= nil and not map:get_game():get_value("519") then
    i = tonumber(i)
    if puzzle_next_sensor == 5 and i == 1 then
      puzzle_solved()
    elseif i == puzzle_next_sensor then
      puzzle_next_sensor = puzzle_next_sensor + 1
    else
      puzzle_next_sensor = 1
    end
  end
end
for _, sensor in ipairs(map:get_entities("puzzle_sensor_")) do
  sensor.on_activated = puzzle_sensor_activated
end

function statue_switch:on_activated()

  if not statue_door:is_open() then
    map:move_camera(432, 536, 250, function()
      sol.audio.play_sound("secret")
      sol.audio.play_sound("door_open")
      map:open_doors("statue_door")
    end)
  end
end

function map:on_update()

  if not torches_door:is_open()
      and are_all_torches_on() then

    sol.audio.play_sound("secret")
    map:open_doors("torches_door")
    lock_torches()
  end
end

function weak_floor_sensor:on_collision_explosion()

  if weak_floor:is_enabled() then

    weak_floor:set_enabled(false)
    weak_floor_sensor:set_enabled(false)
    sol.audio.play_sound("secret")
    map:get_game():set_value("515", true)
    sol.timer.start(1500, function()
      weak_floor_block:set_enabled(false)
      sol.audio.play_sound("jump")
      sol.timer.start(200, function()
	sol.audio.play_sound("bomb")
      end)
    end)
  end
end

