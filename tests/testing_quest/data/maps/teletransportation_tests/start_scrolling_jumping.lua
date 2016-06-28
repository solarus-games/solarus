local map = ...
local game = map:get_game()

function map:on_started()

  hero:assert_position_equal(sensor_on_started)
  hero:assert_state_ground_animation("jumping", "empty", "jumping")
end

function map:on_opening_transition_finished()

  hero:assert_position_equal(sensor_on_opening_transition_finished)
  hero:assert_state_ground_animation("jumping", "traversable", "jumping")
end

function go_down_sensor:on_activated()

  hero:assert_state_ground_animation("jumping", "traversable", "jumping")
  game:simulate_command_pressed("down")
end

function go_left_sensor:on_activated()

  hero:assert_state_ground_animation("free", "traversable", "walking")
  game:simulate_command_released("down")
  game:simulate_command_pressed("left")
  sol.timer.start(100, function()
    game:simulate_command_pressed("action")  -- Lift the pot.
  end)
end
