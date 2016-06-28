local map = ...
local game = map:get_game()

-- Starting a map in a hole:
-- The hero should fall only after the opening transition.

function map:on_started()

  hero:assert_position_equal(hero, destination_1)
  hero:assert_state_ground_animation("free", "hole", "stopped")
end

function map:on_opening_transition_finished()

  hero:assert_position_equal(hero, destination_1)
  hero:assert_state_ground_animation("falling", "hole", "falling")

  sol.timer.start(500, function()

    hero:assert_position_equal(hero, destination_2)
    hero:assert_state_ground_animation("falling", "hole", "falling")
  end)
end
