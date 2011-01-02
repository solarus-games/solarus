-------------------------
-- Dungeon 1 1F script --
-------------------------

current_room = ""
sw_sensor_enabled = false

function event_map_started(destination_point_name)

  sol.map.chest_set_enabled("map_chest", false)
  sol.map.chest_set_enabled("compass_chest", false)
  if sol.game.savegame_get_boolean(54) then
    sol.map.switch_set_activated("map_room_switch", true)
  end
end

function event_map_opening_transition_finished(destination_point_name)

  -- show the welcome message
  if destination_point_name == "from_outside" then
    sol.map.dialog_start("dungeon_1")
  end
end

function event_switch_activated(switch_name)

  if switch_name == "sw_switch" or switch_name == "nw_switch" then
    sol.map.camera_move(176, 392, 150)
    current_room = "sw"
  elseif switch_name == "map_room_switch" then
    sol.main.play_sound("chest_appears")
    sol.map.chest_set_enabled("map_chest", true)
  end
end

function event_camera_reached_target()
  if current_room == "sw" then
    sol.main.timer_start(1000, "sw_camera_timer", false)
  elseif current_room == "compass_room" then
    sol.main.timer_start(1000, "compass_room_timer", false)
  end
end

function sw_camera_timer()
  open_sw_door()
  sol.main.timer_start(1000, "sol.map.camera_restore", false)
end

function compass_room_timer()
  sol.main.play_sound("chest_appears")
  sol.map.chest_set_enabled("compass_chest", true)
  sol.main.timer_start(1000, "sol.map.camera_restore", false)
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "close_sw_door_sensor"
    and sol.map.door_is_open("sw_door")
    and sw_sensor_enabled then

    close_sw_door()
    sw_sensor_enabled = false
    sol.map.hero_align_on_sensor(sensor_name)

  elseif sensor_name == "enable_sensor" then
    sw_sensor_enabled = true
  end
end

function open_sw_door()
  sol.main.play_sound("secret")
  sol.map.door_open("sw_door")
  sol.map.switch_set_activated("sw_switch", true)
  sol.map.switch_set_activated("nw_switch", true)
end

function close_sw_door()
  sol.map.door_close("sw_door")
  sol.map.switch_set_activated("sw_switch", false)
  sol.map.switch_set_activated("nw_switch", false)
end

function event_enemy_dead(enemy_name)

  if sol.map.enemy_is_group_dead("compass_room_battle")
    and not sol.map.chest_is_enabled("compass_chest") then
    sol.map.camera_move(408, 456, 150)
    current_room = "compass_room"
  end
end

