local map = ...
-- Lost Woods cave

function map:on_switch_activated(switch_name)

  if string.find(switch_name, "^torch_a_switch") then
    map:move_camera(176, 264, 250, remove_torch_a)
  elseif string.find(switch_name, "^torch_b_switch") then
    map:move_camera(224, 264, 250, remove_torch_b)
  elseif switch_name == "torch_c_switch" then
    map:move_camera(608, 192, 250, remove_torch_c)
  elseif switch_name == "torch_d_switch" then
    map:move_camera(584, 48, 250, remove_torch_d)
  elseif switch_name == "torch_e_switch" then
    map:move_camera(512, 456, 250, remove_torch_e)
  end
end

function map:on_hero_on_sensor(sensor_name)

  if sensor_name == "close_torch_a_sensor" then
    if not torch_a:is_enabled()
        and not torch_a_switch_2:is_activated() then
      put_torch_a()
    end
  elseif sensor_name == "close_torch_b_sensor" then
    if not torch_b:is_enabled()
        and not torch_b_switch_2:is_activated() then
      put_torch_b()
    end
  end
end

function remove_torch_a()
  sol.audio.play_sound("door_open")
  sol.audio.play_sound("secret")
  map:set_entities_enabled("torch_a", false)
  torch_a_switch:set_activated(true)
end

function remove_torch_b()
  sol.audio.play_sound("door_open")
  sol.audio.play_sound("secret")
  map:set_entities_enabled("torch_b", false)
  torch_b_switch:set_activated(true)
end

function remove_torch_c()
  sol.audio.play_sound("door_open")
  sol.audio.play_sound("secret")
  map:set_entities_enabled("torch_c", false)
end

function remove_torch_d()
  sol.audio.play_sound("door_open")
  sol.audio.play_sound("secret")
  map:set_entities_enabled("torch_d", false)
end

function remove_torch_e()
  sol.audio.play_sound("door_open")
  sol.audio.play_sound("secret")
  map:set_entities_enabled("torch_e", false)
end

function put_torch_a()
  sol.audio.play_sound("door_closed")
  map:set_entities_enabled("torch_a", true)
  torch_a_switch:set_activated(false)
  torch_a_switch_2:set_activated(false)
end

function put_torch_b()
  sol.audio.play_sound("door_closed")
  map:set_entities_enabled("torch_b", true)
  torch_b_switch:set_activated(false)
  torch_b_switch_2:set_activated(false)
end

