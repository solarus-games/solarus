local map = ...
local game = map:get_game()

function map:on_started()

  game:set_ability("sword", 1)

  hero:assert_position_equal(sensor_on_started)
  hero:assert_state_ground_animation("running", "empty", "sword_loading_walking")
end

function map:on_opening_transition_finished()

  hero:assert_position_equal(sensor_on_opening_transition_finished)
  hero:assert_state_ground_animation("running", "traversable", "sword_loading_walking")
end

function go_down_sensor:on_activated()

  -- Stop running.
  hero:freeze()
  hero:unfreeze()

  -- Use the sword.
  game:simulate_command_pressed("attack")
  game:simulate_command_pressed("down")

  sol.timer.start(2000, function()
    hero:assert_state_ground_animation("sword loading", "traversable", "sword_loading_walking")
  end)
end
