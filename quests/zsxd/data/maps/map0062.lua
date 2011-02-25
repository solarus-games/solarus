-- Temple of Stupidities 1F NW

-- stupid run between switches and doors
timer = nil
timer_delay = 0
door_a_passed = false
door_b_passed = false
door_c_passed = false

function event_switch_activated(switch_name)

  -- when pressing a switch, move the camera and open a door
  if switch_name == "stupid_run_switch_a" then
    timer = "open_door_a"
    sol.map.camera_move(1272, 152, 250)
  elseif switch_name == "stupid_run_switch_b" then
    timer = "open_door_b"
    sol.map.camera_move(1288, 296, 250)
  elseif switch_name == "stupid_run_switch_c" then
    timer = "open_door_c"
    sol.map.camera_move(1456, 320, 250)
  end
end

function event_camera_reached_target()
  sol.main.timer_start(1000, timer, false)
end

function event_camera_back()
  -- start the door timer once the camera is back
  sol.main.timer_start(timer_delay, timer, true)
end

function event_hero_on_sensor(sensor_name)

  -- sensors that close doors
  if sensor_name == "close_door_b_sensor"
    and door_b_passed
    and sol.map.door_is_open("stupid_run_door_b") then
    close_door_b()
  elseif sensor_name == "close_door_c_sensor"
    and door_c_passed
    and sol.map.door_is_open("stupid_run_door_c") then
    close_door_c()

  -- sensors that stop timers (i.e. a door is reached on time)
  elseif string.match(sensor_name, "^stop_timer_a_sensor") then
    sol.main.timer_stop("close_door_a")
    door_a_passed = true
  elseif string.match(sensor_name, "^stop_timer_b_sensor") then
    sol.main.timer_stop("close_door_b")
    door_b_passed = true
  elseif string.match(sensor_name, "^stop_timer_c_sensor") then
    sol.main.timer_stop("close_door_c")
    door_c_passed = true
  end
end

function open_door_a()
  sol.map.door_open("stupid_run_door_a")
  sol.main.timer_start(1000, "sol.map.camera_restore", false)
  timer = "close_door_a"
  timer_delay = 7000
end

function open_door_b()
  sol.map.door_open("stupid_run_door_b")
  sol.main.timer_start(1000, "sol.map.camera_restore", false)
  timer = "close_door_b"
  timer_delay = 3500
end

function open_door_c()
  sol.map.door_open("stupid_run_door_c")
  sol.main.timer_start(1000, "sol.map.camera_restore", false)
  timer = "close_door_c"
  timer_delay = 4000
end

function close_door_a()
  sol.map.door_close("stupid_run_door_a")
  sol.map.switch_set_activated("stupid_run_switch_a", false)
  door_a_passed = false
end

function close_door_b()
  sol.map.door_close("stupid_run_door_b")
  sol.map.switch_set_activated("stupid_run_switch_b", false)
  door_b_passed = false
end

function close_door_c()
  sol.map.door_close("stupid_run_door_c")
  sol.map.switch_set_activated("stupid_run_switch_c", false)
  door_c_passed = false
end

