-- Dungeon 4 B1

function event_switch_activated(switch_name)

  if switch_name == "door_a_switch" then
    sol.map.door_open("door_a")
    sol.main.play_sound("secret")
  elseif switch_name == "door_b_switch" then
    sol.map.door_open("door_b")
    sol.main.play_sound("secret")
  elseif switch_name == "special_torch_switch" then
    sol.map.tile_set_group_enabled("special_torch", false)
    sol.main.play_sound("secret")
 end
end

function event_switch_inactivated(switch_name)

  if switch_name == "door_b_switch" then
    sol.map.door_close("door_b")
  elseif switch_name == "special_torch_switch" then
    sol.map.tile_set_group_enabled("special_torch", true)
    sol.main.play_sound("door_closed")
  end
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "close_door_a_sensor"
      and sol.map.door_is_open("door_a")
      and not sol.map.door_is_open("door_b") then
    sol.map.door_close("door_a")
    sol.map.switch_set_activated("door_a_switch", false)
    sol.map.tile_set_group_enabled("special_torch", true)
  end
end

