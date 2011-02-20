-- Temple of Stupidities 1F NE

function event_switch_activated(switch_name)

  if switch_name == "switch_torch_1_on" then
    sol.map.tile_set_enabled("torch_1", true)
    sol.map.switch_set_activated("switch_torch_1_off", false)
  elseif switch_name == "switch_torch_1_off" then
    sol.map.tile_set_enabled("torch_1", false)
    sol.map.switch_set_activated("switch_torch_1_on", false)
  elseif switch_name == "switch_torch_2_on" then
    sol.map.tile_set_enabled("torch_2", true)
    sol.map.switch_set_activated("switch_torch_2_off", false)
  elseif switch_name == "switch_torch_2_off" then
    sol.map.tile_set_enabled("torch_2", false)
    sol.map.switch_set_activated("switch_torch_2_on", false)
  elseif switch_name == "switch_torch_3_on" then
    sol.map.tile_set_enabled("torch_3", true)
    sol.map.switch_set_activated("switch_torch_3_off", false)
  elseif switch_name == "switch_torch_3_off" then
    sol.map.tile_set_enabled("torch_3", false)
    sol.map.switch_set_activated("switch_torch_3_on", false)
  elseif switch_name == "switch_torch_4_on" then
    sol.map.tile_set_enabled("torch_4", true)
    sol.map.switch_set_activated("switch_torch_4_off", false)
  elseif switch_name == "switch_torch_4_off" then
    sol.map.tile_set_enabled("torch_4", false)
    sol.map.switch_set_activated("switch_torch_4_on", false)
  elseif switch_name == "switch_torch_5_on" then
    sol.map.tile_set_enabled("torch_5", true)
    sol.map.switch_set_activated("switch_torch_5_off", false)
  elseif switch_name == "switch_torch_5_off" then
    sol.map.tile_set_enabled("torch_5", false)
    sol.map.switch_set_activated("switch_torch_5_on", false)
  elseif switch_name == "switch_torch_6_on" then
    sol.map.tile_set_enabled("torch_6", true)
    sol.map.switch_set_activated("switch_torch_6_off", false)
  elseif switch_name == "switch_torch_6_off" then
    sol.map.tile_set_enabled("torch_6", false)
    sol.map.switch_set_activated("switch_torch_6_on", false)
  elseif switch_name == "switch_torch_7_on" then
    sol.map.tile_set_enabled("torch_7", true)
    sol.map.switch_set_activated("switch_torch_7_off", false)
  elseif switch_name == "switch_torch_7_off"
      or switch_name == "switch_torch_7_off_2" then
    sol.map.tile_set_enabled("torch_7", false)
    sol.map.switch_set_activated("switch_torch_7_on", false)
  elseif switch_name == "switch_torch_8_on" then
    sol.map.tile_set_enabled("torch_8", true)
    sol.map.switch_set_activated("switch_torch_8_off", false)
  elseif switch_name == "switch_torch_8_off" then
    sol.map.tile_set_enabled("torch_8", false)
    sol.map.switch_set_activated("switch_torch_8_on", false)
  elseif switch_name == "switch_torch_9_on" then
    sol.map.tile_set_enabled("torch_9", true)
    sol.map.switch_set_activated("switch_torch_9_off", false)
  elseif switch_name == "switch_torch_9_off" then
    sol.map.tile_set_enabled("torch_9", false)
    sol.map.switch_set_activated("switch_torch_9_on", false)
  elseif switch_name == "switch_torch_10_on" then
    sol.map.tile_set_enabled("torch_10", true)
    sol.map.switch_set_activated("switch_torch_10_off", false)
  elseif switch_name == "switch_torch_10_off" then
    sol.map.tile_set_enabled("torch_10", false)
    sol.map.switch_set_activated("switch_torch_10_on", false)
  elseif switch_name == "switch_torch_11_on" then
    sol.map.tile_set_enabled("torch_11", true)
    sol.map.switch_set_activated("switch_torch_11_off", false)
  elseif switch_name == "switch_torch_11_off" then
    sol.map.tile_set_enabled("torch_11", false)
    sol.map.switch_set_activated("switch_torch_11_on", false)
  end
end


