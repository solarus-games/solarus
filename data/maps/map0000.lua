function event_map_started()
   start_timer(10000, "timer_end", true)
--   start_message("msg")
--   set_message_variable("test_var", 3)
--   set_message_variable("test_name", "Midjy")
end

function timer_end()
   play_sound("door_closed")
end
