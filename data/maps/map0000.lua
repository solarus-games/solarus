function event_map_started()
--   start_timer(1000, "timer_end", true)
end

function timer_end()
   play_sound("door_closed")
   start_message("msg")
   set_message_variable("test_var", 3)
   set_message_variable("test_name", "Midjy")
end
