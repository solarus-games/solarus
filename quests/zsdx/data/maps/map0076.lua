local map = ...
-- Lost Woods cave

function map:on_switch_activated(switch_name)

  if string.find(switch_name, "^torch_a_switch") then
    map:camera_move(176, 264, 250, remove_torch_a)
  elseif string.find(switch_name, "^torch_b_switch") then
    map:camera_move(224, 264, 250, remove_torch_b)
  elseif switch_name == "torch_c_switch" then
    map:camera_move(608, 192, 250, remove_torch_c)
  elseif switch_name == "torch_d_switch" then
    map:camera_move(584, 48, 250, remove_torch_d)
  elseif switch_name == "torch_e_switch" then
    map:camera_move(512, 456, 250, remove_torch_e)
  end
end

function map:on_hero_on_sensor(sensor_name)

  if sensor_name == "close_torch_a_sensor" then
    if not map:tile_is_enabled("torch_a")
        and not map:switch_is_activated("torch_a_switch_2") then
      put_torch_a()
    end
  elseif sensor_name == "close_torch_b_sensor" then
    if not map:tile_is_enabled("torch_b")
        and not map:switch_is_activated("torch_b_switch_2") then
      put_torch_b()
    end
  end
end

function remove_torch_a()
  sol.audio.play_sound("door_open")
  sol.audio.play_sound("secret")
  map:tile_set_group_enabled("torch_a", false)
  map:switch_set_activated("torch_a_switch", true)
end

function remove_torch_b()
  sol.audio.play_sound("door_open")
  sol.audio.play_sound("secret")
  map:tile_set_group_enabled("torch_b", false)
  map:switch_set_activated("torch_b_switch", true)
end

function remove_torch_c()
  sol.audio.play_sound("door_open")
  sol.audio.play_sound("secret")
  map:tile_set_group_enabled("torch_c", false)
end

function remove_torch_d()
  sol.audio.play_sound("door_open")
  sol.audio.play_sound("secret")
  map:tile_set_group_enabled("torch_d", false)
end

function remove_torch_e()
  sol.audio.play_sound("door_open")
  sol.audio.play_sound("secret")
  map:tile_set_group_enabled("torch_e", false)
end

function put_torch_a()
  sol.audio.play_sound("door_closed")
  map:tile_set_group_enabled("torch_a", true)
  map:switch_set_activated("torch_a_switch", false)
  map:switch_set_activated("torch_a_switch_2", false)
end

function put_torch_b()
  sol.audio.play_sound("door_closed")
  map:tile_set_group_enabled("torch_b", true)
  map:switch_set_activated("torch_b_switch", false)
  map:switch_set_activated("torch_b_switch_2", false)
end

