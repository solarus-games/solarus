function event_map_started()
--   move_camera(928, 416, 12)
end

function event_camera_reached_target()
   start_timer(1000, "timer", false)
end

function timer()
   restore_camera()
end