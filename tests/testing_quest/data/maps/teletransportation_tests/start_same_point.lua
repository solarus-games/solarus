local map = ...
local game = map:get_game()

function map:on_started()

  hero:assert_position_equal(sensor)
  hero:assert_state_ground_animation("free", "traversable", "stopped")
end

function map:on_opening_transition_finished()

  hero:set_position(destination_1:get_position())
  game:simulate_command_pressed("right")
end
