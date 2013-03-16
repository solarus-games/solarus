local map = ...
-- This script is included by the maps of dungeon 5.
-- It handles sending the hero to prison when a guard sees him.

local hero = map:get_entity("hero")
local hero_seen = false
local prison_go_timer = nil
local prison_dialog_timer = nil

local function seen_by_guard(guard)

  hero_seen = true
  hero:freeze()
  guard:get_sprite():set_animation("walking")
  local m = sol.movement.create("target")
  m:set_speed(96)
  m:start(guard)
  sol.audio.play_sound("hero_seen")
  prison_dialog_timer = sol.timer.start(500, function()
    if prison_dialog_timer ~= nil then
      prison_dialog_timer:stop()
      prison_dialog_timer = nil
    end
    if prison_go_timer ~= nil then
      prison_go_timer:stop()
      prison_go_timer = nil
    end
    map:start_dialog("dungeon_5.hero_seen_by_guard", function()
      prison_go_timer = sol.timer.start(1000, function()
	hero_seen = false
	if prison_dialog_timer ~= nil then
	  prison_dialog_timer:stop()
	  prison_dialog_timer = nil
	end
	prison_go_timer = nil
	hero:teleport(65, "prison")
	if map.init_prison ~= nil then
	  -- special case: we are already on the map of the prison
	  map:init_prison()
	end
      end)
    end)
  end)
end

local function guard_sensor_activated(sensor)

  local guard_num, direction = sensor:get_name():match("guard_([0-9])_sensor_([0-3])")
  if not hero_seen and guard_num ~= nil then
    local guard = map:get_entity("guard_" .. guard_num)
    local sprite = guard:get_sprite()
    direction = tonumber(direction)
    if sprite:get_direction() == direction then

      local x, y = guard:get_position()
      local hero_x, hero_y = hero:get_position()
      if direction == 0 and hero_x > x - 32 and hero_x < x + 216 then
        seen_by_guard(guard)
      elseif direction == 1 and hero_y < y + 32 and hero_y > y - 216 then
        seen_by_guard(guard)
      elseif direction == 2 and hero_x < x + 32 and hero_x > x - 216 then
        seen_by_guard(guard)
      elseif direction == 3 and hero_y > y - 32 and hero_y < y + 216 then
        seen_by_guard(guard)
      end
    end
  end
end

local guards = map:get_entities("guard_")
for _, guard in ipairs(guards) do
  local sensors = map:get_entities(guard:get_name() .. "_sensor_")
  for _, sensor in ipairs(sensors) do
    sensor.on_activated = guard_sensor_activated
    sensor.on_activated_repeat = guard_sensor_activated
  end
end

function map:cancel_prison()
  if prison_dialog_timer ~= nil then
    prison_dialog_timer:stop()
    prison_dialog_timer = nil
  end
  if prison_go_timer ~= nil then
    prison_go_timer:stop()
    prison_go_timer = nil
  end
  hero_seen = false
end

