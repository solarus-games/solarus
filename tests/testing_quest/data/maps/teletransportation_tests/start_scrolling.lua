local map = ...
local game = map:get_game()

function map:on_started()

  hero:assert_position_equal(sensor_on_started)
  hero:assert_state_ground_animation("free", "empty", "stopped")
  -- TODO fix initial ground after scrolling
end

function map:on_opening_transition_finished()

  hero:assert_position_equal(sensor_on_opening_transition_finished)
  hero:assert_state_ground_animation("free", "traversable", "stopped")
end

function go_south_sensor:on_activated()

  game:simulate_command_released("right")
  game:simulate_command_pressed("down")
end

function jump_sensor:on_activated()

  game:simulate_command_released("down")
  hero:start_jumping(6, 48)
end
