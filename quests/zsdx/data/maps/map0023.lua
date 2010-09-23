-------------------------
-- Dungeon 1 1F script --
-------------------------

current_room = ""
sw_sensor_enabled = false

function event_map_started(destination_point_name)

  chest_set_hidden("map_chest", true)
  chest_set_hidden("compass_chest", true)
  if savegame_get_boolean(54) then
    switch_set_enabled("map_room_switch", true)
  end
end

function event_switch_enabled(switch_name)

  if switch_name == "sw_switch" or switch_name == "nw_switch" then
    camera_move(176, 392, 15)
    current_room = "sw"
  elseif switch_name == "map_room_switch" then
    play_sound("chest_appears")
    chest_set_hidden("map_chest", false)
  end
end

function event_camera_reached_target()
  if current_room == "sw" then
    timer_start(1000, "sw_camera_timer", false)
  elseif current_room == "compass_room" then
    timer_start(1000, "compass_room_timer", false)
  end
end

function sw_camera_timer()
  open_sw_door()
  timer_start(1000, "camera_restore", false)
end

function compass_room_timer()
  play_sound("chest_appears")
  chest_set_hidden("compass_chest", false)
  timer_start(1000, "camera_restore", false)
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "close_sw_door_sensor"
    and door_is_open("sw_door")
    and sw_sensor_enabled then

    close_sw_door()
    sw_sensor_enabled = false
    hero_align_on_sensor(sensor_name)

  elseif sensor_name == "enable_sensor" then
    sw_sensor_enabled = true
  end
end

function open_sw_door()
  play_sound("secret")
  door_open("sw_door")
  switch_set_enabled("sw_switch", true)
  switch_set_enabled("nw_switch", true)
end

function close_sw_door()
  door_close("sw_door")
  switch_set_enabled("sw_switch", false)
  switch_set_enabled("nw_switch", false)
end

function event_enemy_dead(enemy_name)

  if enemy_is_group_dead("compass_room_battle") and chest_is_hidden("compass_chest") then
    camera_move(408, 456, 15)
    current_room = "compass_room"
  end
end

