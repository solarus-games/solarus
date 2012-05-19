-- This script is included by the maps of dungeon 5.
-- It handles sending the hero to prison when a guard sees him.

local guard_name = nil
local hero_seen = false
local prison_go_timer = nil
local prison_dialog_timer = nil

-- This function should be called by event_dialog_finished in your map.
function dialog_check_guard(dialog_id)

  if dialog_id == "dungeon_5.hero_seen_by_guard" then
    prison_go_timer = sol.main:start_timer(1000, send_hero_to_prison)
  end
end

-- This function should be called by event_hero_on_sensor in your map.
function sensor_check_guard(sensor_name)

  local guard_num, direction = string.match(sensor_name,
    "guard_([0-9])_sensor_([0-3])")
  if not hero_seen and guard_num ~= nil then
    guard_name = "guard_"..guard_num
    local sprite = sol.map.npc_get_sprite(guard_name)
    direction = tonumber(direction)
    if sprite:get_direction() == direction then

      local x, y = sol.map.npc_get_position(guard_name)
      local hero_x, hero_y = sol.map.hero_get_position()
      if direction == 0 and hero_x > x - 32 and hero_x < x + 216 then
        seen_by_guard(guard_name)
      elseif direction == 1 and hero_y < y + 32 and hero_y > y - 216 then
        seen_by_guard(guard_name)
      elseif direction == 2 and hero_x < x + 32 and hero_x > x - 216 then
        seen_by_guard(guard_name)
      elseif direction == 3 and hero_y > y - 32 and hero_y < y + 216 then
        seen_by_guard(guard_name)
      end
    end
  end
end

function seen_by_guard(guard_name)

  hero_seen = true
  sol.map.hero_freeze()
  local sprite = sol.map.npc_get_sprite(guard_name)
  sprite:set_animation("walking")
  local m = sol.movement.create("target")
  m:set_speed(96)
  sol.map.npc_start_movement(guard_name, m)
  prison_dialog_timer = sol.main:start_timer(500, prison_dialog)
  sol.audio.play_sound("hero_seen")
end

function prison_dialog()
  sol.main.timer_stop_all()
  sol.map.dialog_start("dungeon_5.hero_seen_by_guard")
end

function send_hero_to_prison()
  hero_seen = false
  if prison_dialog_timer ~= nil then
    prison_dialog_timer:stop()
    prison_dialog_timer = nil
  end
  prison_go_timer = nil
  sol.map.hero_set_map(65, "prison", 1)
  if init_prison ~= nil then
    -- special case: we are already on the map of the prison
    init_prison()
  end
end

function cancel_prison()
  if prison_dialog_timer ~= nil then
    prison_dialog_timer:stop()
    prison_dialog_timer = nil
  end
  if prison_go_timer ~= nil then
    prison_go_timer:stop()
    prison_go_timer = nil
  end
  hero_seen = false
  guard_name = nil
end

