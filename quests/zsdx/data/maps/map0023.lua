local map = ...
-- Dungeon 1 1F

sw_sensor_enabled = false

function map:on_started(destination_point_name)

  map:chest_set_enabled("map_chest", false)
  map:chest_set_enabled("compass_chest", false)
  if map:get_game():get_boolean(54) then
    map:switch_set_activated("map_room_switch", true)
  end
end

function map:on_map_opening_transition_finished(destination_point_name)

  -- show the welcome message
  if destination_point_name == "from_outside" then
    map:start_dialog("dungeon_1")
  end
end

function map:on_switch_activated(switch_name)

  if switch_name == "sw_switch" or switch_name == "nw_switch" then
    map:move_camera(176, 392, 250, sw_camera_timer)
    current_room = "sw"
  elseif switch_name == "map_room_switch" then
    sol.audio.play_sound("chest_appears")
    map:chest_set_enabled("map_chest", true)
  end
end

function sw_camera_timer()
  open_sw_door()
end

function compass_room_timer()
  sol.audio.play_sound("chest_appears")
  map:chest_set_enabled("compass_chest", true)
end

function map:on_hero_on_sensor(sensor_name)

  if sensor_name == "close_sw_door_sensor"
      and map:door_is_open("sw_door")
      and sw_sensor_enabled then

    close_sw_door()
    sw_sensor_enabled = false
    map:hero_align_on_sensor(sensor_name)

  elseif sensor_name == "enable_sensor" then
    sw_sensor_enabled = true

  elseif sensor_name:find("^save_solid_ground_sensor") then
    map:hero_save_solid_ground()
  end
end

function open_sw_door()
  sol.audio.play_sound("secret")
  map:door_open("sw_door")
  map:switch_set_activated("sw_switch", true)
  map:switch_set_activated("nw_switch", true)
end

function close_sw_door()
  map:door_close("sw_door")
  map:switch_set_activated("sw_switch", false)
  map:switch_set_activated("nw_switch", false)
end

function map:on_enemy_dead(enemy_name)

  if map:enemy_is_group_dead("compass_room_battle")
      and not map:chest_is_enabled("compass_chest") then
    map:move_camera(408, 456, 250, compass_room_timer)
  end
end

function map:on_door_open(door_name)

  if door_name == "weak_wall" then
    sol.audio.play_sound("secret")
  end
end

