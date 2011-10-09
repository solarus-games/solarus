-- This script is included by the maps of dungeon 5.
-- It handles sending the hero to prison when a guard sees him.

guard_name = nil
hero_seen = false

function dialog_check_guard(first_message_id)

  if first_message_id == "dungeon_5.hero_seen_by_guard" then
    sol.main.timer_start(send_hero_to_prison, 1000)
  end
end

function sensor_check_guard(sensor_name)

  local guard_num, direction = string.match(sensor_name,
    "guard_([0-9])_sensor_([0-3])")
  if not hero_seen and guard_num ~= nil then
    guard_name = "guard_"..guard_num
    local sprite = sol.map.npc_get_sprite(guard_name)
    direction = tonumber(direction)
    if sol.main.sprite_get_direction(sprite) == direction then

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
  sol.main.sprite_set_animation(sprite, "walking")
  local m = sol.main.target_movement_create(96)
  sol.map.npc_start_movement(guard_name, m)
  sol.main.timer_start(prison_dialog, 500)
  sol.main.play_sound("hero_seen")
end

function prison_dialog()
  sol.map.dialog_start("dungeon_5.hero_seen_by_guard")
end

function send_hero_to_prison()
  hero_seen = false
  sol.map.hero_set_map(65, "prison", 1)
  if init_prison ~= nil then
    -- special case: we are already on the map of the prison
    init_prison()
  end
end

