-- Dungeon 4 B1

function event_map_started(destination_point_name)

  x,y = sol.map.hero_get_position()
  if y < 520 then
    -- the player is coming from the north part of the map:
    -- place the block so that the special torch is already disabled
    sol.map.tile_set_group_enabled("special_torch", false)
    sol.map.switch_set_activated("special_torch_switch", true)
    sol.map.block_set_position("block", 752, 453, 1);
  end
end

function event_switch_activated(switch_name)

  if switch_name == "door_a_switch" then
    sol.map.door_open("door_a")
    sol.main.play_sound("secret")
  elseif switch_name == "door_b_switch" then
    sol.map.door_open("door_b")
    sol.main.play_sound("secret")
  elseif switch_name == "special_torch_switch" then
    if sol.map.tile_is_enabled("special_torch") then
      sol.map.tile_set_group_enabled("special_torch", false)
      sol.main.play_sound("secret")
    end
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
  elseif sensor_name == "sensor_1" and sol.map.tile_is_enabled("sensor_1_off") then
    sol.main.play_sound("switch")
    sol.map.tile_set_group_enabled("sensor_1_on", true)
    sol.map.tile_set_group_enabled("sensor_1_off", false)
  elseif sensor_name == "sensor_2" and sol.map.tile_is_enabled("sensor_2_off") then
    sol.main.play_sound("switch")
    sol.map.tile_set_group_enabled("sensor_2_on", true)
    sol.map.tile_set_group_enabled("sensor_2_off", false)
  elseif sensor_name == "sensor_3" and sol.map.tile_is_enabled("sensor_3_off") then
    sol.main.play_sound("switch")
    sol.map.tile_set_group_enabled("sensor_3_off", false)
  elseif sensor_name == "sensor_4" then
    sol.main.play_sound("switch")
    sol.map.tile_set_group_enabled("sensor_4_on", not sol.map.tile_is_enabled("sensor_4_on"))
  end
end

