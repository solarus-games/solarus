-- Lost Woods cave

camera_timer = ""

function event_switch_activated(switch_name)

  if string.match(switch_name, "^torch_a_switch") then
    camera_timer = "remove_torch_a"
    sol.map.camera_move(176, 264, 250)
  elseif string.match(switch_name, "^torch_b_switch") then
    camera_timer = "remove_torch_b"
    sol.map.camera_move(224, 264, 250)
  elseif switch_name == "torch_c_switch" then
    camera_timer = "remove_torch_c"
    sol.map.camera_move(608, 192, 250)
  elseif switch_name == "torch_d_switch" then
    camera_timer = "remove_torch_d"
    sol.map.camera_move(584, 48, 250)
  elseif switch_name == "torch_e_switch" then
    camera_timer = "remove_torch_e"
    sol.map.camera_move(512, 456, 250)
  end
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "close_torch_a_sensor" then
    if not sol.map.tile_is_enabled("torch_a")
        and not sol.map.switch_is_activated("torch_a_switch_2") then
      put_torch_a()
    end
  elseif sensor_name == "close_torch_b_sensor" then
    if not sol.map.tile_is_enabled("torch_b")
        and not sol.map.switch_is_activated("torch_b_switch_2") then
      put_torch_b()
    end
  end
end

function event_camera_reached_target()
  sol.main.timer_start(1000, camera_timer, false)
end

function remove_torch_a()
  sol.main.play_sound("door_open")
  sol.main.play_sound("secret")
  sol.map.tile_set_group_enabled("torch_a", false)
  sol.map.switch_set_activated("torch_a_switch", true)
  sol.main.timer_start(1000, "sol.map.camera_restore", false)
end

function remove_torch_b()
  sol.main.play_sound("door_open")
  sol.main.play_sound("secret")
  sol.map.tile_set_group_enabled("torch_b", false)
  sol.map.switch_set_activated("torch_b_switch", true)
  sol.main.timer_start(1000, "sol.map.camera_restore", false)
end

function remove_torch_c()
  sol.main.play_sound("door_open")
  sol.main.play_sound("secret")
  sol.map.tile_set_group_enabled("torch_c", false)
  sol.main.timer_start(1000, "sol.map.camera_restore", false)
end

function remove_torch_d()
  sol.main.play_sound("door_open")
  sol.main.play_sound("secret")
  sol.map.tile_set_group_enabled("torch_d", false)
  sol.main.timer_start(1000, "sol.map.camera_restore", false)
end

function remove_torch_e()
  sol.main.play_sound("door_open")
  sol.main.play_sound("secret")
  sol.map.tile_set_group_enabled("torch_e", false)
  sol.main.timer_start(1000, "sol.map.camera_restore", false)
end

function put_torch_a()
  sol.main.play_sound("door_closed")
  sol.map.tile_set_group_enabled("torch_a", true)
  sol.map.switch_set_activated("torch_a_switch", false)
  sol.map.switch_set_activated("torch_a_switch_2", false)
end

function put_torch_b()
  sol.main.play_sound("door_closed")
  sol.map.tile_set_group_enabled("torch_b", true)
  sol.map.switch_set_activated("torch_b_switch", false)
  sol.map.switch_set_activated("torch_b_switch_2", false)
end

