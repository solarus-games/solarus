-- Temple of Stupidities 1F NW

-- stupid run between switches and doors
local timer = nil
local timer_delay = 0
local door_a_passed = false
local door_b_passed = false
local door_c_passed = false

function event_switch_activated(switch_name)

  -- when pressing a switch, move the camera and open a door
  if switch_name == "stupid_run_switch_a" then
    sol.map.camera_move(1272, 152, 250, open_door_a)
  elseif switch_name == "stupid_run_switch_b" then
    sol.map.camera_move(1288, 296, 250, open_door_b)
  elseif switch_name == "stupid_run_switch_c" then
    sol.map.camera_move(1456, 320, 250, open_door_c)
  end
end

function event_camera_back()
  -- start the door timer once the camera is back
  sol.main.timer_start(timer, timer_delay, true)
end

function event_hero_on_sensor(sensor_name)

  -- sensors that close doors
  if sensor_name == "close_door_b_sensor"
      and door_b_passed
      and sol.map.door_is_open("stupid_run_door_b") then
      door_b_passed = false
    close_door_b()
  elseif sensor_name == "close_door_c_sensor"
      and door_c_passed
      and sol.map.door_is_open("stupid_run_door_c") then
    door_c_passed = false
    close_door_c()

  -- sensors that stop timers (i.e. a door is reached on time)
  elseif string.find(sensor_name, "^stop_timer_a_sensor") then
    door_a_passed = true
  elseif string.find(sensor_name, "^stop_timer_b_sensor") then
    door_b_passed = true
  elseif string.find(sensor_name, "^stop_timer_c_sensor") then
    door_c_passed = true
  end
end

function event_treasure_obtained(item_name, variant, savegame_variable)

  if item_name == "map" then
    sol.map.dialog_start("dungeon_1.after_map_treasure")
  end
end

function open_door_a()

  sol.map.door_open("stupid_run_door_a")
  timer = close_door_a
  timer_delay = 7000
end

function open_door_b()

  sol.map.door_open("stupid_run_door_b")
  timer = close_door_b
  timer_delay = 3500
end

function open_door_c()

  sol.map.door_open("stupid_run_door_c")
  timer = close_door_c
  timer_delay = 4000
end

function close_door_a()

  if sol.map.door_is_open("stupid_run_door_a") 
      and not door_a_passed then
    sol.map.door_close("stupid_run_door_a")
    sol.map.switch_set_activated("stupid_run_switch_a", false)
  end
end

function close_door_b()

  if sol.map.door_is_open("stupid_run_door_b") 
      and not door_b_passed then
    sol.map.door_close("stupid_run_door_b")
    sol.map.switch_set_activated("stupid_run_switch_b", false)
  end
end

function close_door_c()

  if sol.map.door_is_open("stupid_run_door_c") 
      and not door_c_passed then
    sol.map.door_close("stupid_run_door_c")
    sol.map.switch_set_activated("stupid_run_switch_c", false)
  end
end

