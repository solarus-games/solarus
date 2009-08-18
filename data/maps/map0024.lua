-------------------------
-- Dungeon 1 B1 script --
-------------------------

function event_map_started(destination_point_name)

  if destination_point_name == "from_1F_hole" then
    enemy_set_enabled("boss", true)
  end
end

function event_switch_enabled(switch_name)

  if switch_name == "se_switch" or switch_name == "ne_switch" then
    move_camera(960, 496, 15)
  end
end

function event_camera_reached_target()
  start_timer(1000, "se_room_timer", false)
end

function se_room_timer()
  open_se_door()
  start_timer(1000, "restore_camera", false)
end

function open_se_door()
  play_sound("secret")
  door_open("se_door")
  switch_set_enabled("se_switch", true)
  switch_set_enabled("ne_switch", true)
end


