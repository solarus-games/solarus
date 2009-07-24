-------------------------
-- Dungeon 1 1F script --
-------------------------

current_room = ""
sw_sensor_enabled = true

function event_switch_enabled(switch_name)

  if switch_name == "sw_switch" or switch_name == "nw_switch" then
    move_camera(104, 392, 15)
    current_room = "sw"
  end
end

function event_camera_reached_target()
  if current_room == "sw" then
    start_timer(1000, "sw_camera_timer", false)
  end
end

function sw_camera_timer()
  open_sw_door()
  start_timer(1000, "restore_camera", false)
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "close_sw_door_sensor"
    and door_is_open("sw_door")
    and sw_sensor_enabled then

    close_sw_door()
    sw_sensor_enabled = false

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

