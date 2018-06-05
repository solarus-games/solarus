local map = ...
local game = map:get_game()

function map:on_started()

  hero:assert_position_equal(sensor_on_started)
  hero:assert_state_ground_animation("sword loading", "empty", "sword_loading_stopped")
end

function map:on_opening_transition_finished()

  hero:assert_position_equal(sensor_on_opening_transition_finished)
  hero:assert_state_ground_animation("sword loading", "traversable", "sword_loading_stopped")
end

function go_right_sensor:on_activated()
  hero:freeze()
  hero:unfreeze()

  game:simulate_command_released("down")
  game:simulate_command_pressed("right")
end
