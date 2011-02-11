-- Dungeon 3 2F

camera_timer = nil

function event_enemy_dead(enemy_name)

  if string.match(enemy_name, "^e_room_enemy_")
    and sol.map.enemy_is_group_dead("e_room_enemy")
    and not sol.map.door_is_open("e_door") then

    camera_timer = "open_e_door"
    sol.map.camera_move(856, 472, 150)
  end
end

function event_camera_reached_target()
  sol.main.timer_start(1000, camera_timer, false)
end

function open_e_door()
  sol.main.play_sound("secret")
  sol.map.door_open("e_door")
  sol.main.timer_start(1000, "sol.map.camera_restore", false)
end

