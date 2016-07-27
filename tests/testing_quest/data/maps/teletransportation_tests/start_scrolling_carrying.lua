local map = ...
local game = map:get_game()

function map:on_started()

  hero:assert_position_equal(sensor_on_started)
  hero:assert_state_ground_animation("carrying", "empty", "carrying_stopped")
end

function map:on_opening_transition_finished()

  hero:assert_position_equal(sensor_on_opening_transition_finished)
  hero:assert_state_ground_animation("carrying", "traversable", "carrying_stopped")
end

function run_sensor:on_activated()

  game:simulate_command_released("left")
  game:simulate_command_pressed("action")  -- Throw the pot.
  game:simulate_command_released("action")
  hero:start_running()
  game:simulate_command_pressed("action")  -- Keep running.
end
