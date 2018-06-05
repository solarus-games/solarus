local map = ...
local game = map:get_game()

local turned = false

function map:on_started()
  hero:assert_position_equal(sensor_on_started)
end

function map:on_opening_transition_finished()
  if not turned then --two entry points
    hero:assert_position_equal(sensor_on_opening_transition_finished)
  else
    hero:assert_position_equal(sensor_on_opening_transition_finished_2)
  end
end

function end_sensor:on_activated()
  sol.main.exit()
end

function go_up_sensor:on_activated()
  hero:freeze()
  hero:unfreeze()

  game:simulate_command_released("right")
  game:simulate_command_pressed("up")
  turned = true
end