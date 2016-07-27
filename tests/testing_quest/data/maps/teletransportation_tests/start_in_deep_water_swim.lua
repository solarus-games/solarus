local map = ...
local game = map:get_game()

function map:on_started()

  game:set_ability("swim", 1)

  hero:assert_position_equal(hero, destination_1)
  hero:assert_state_ground_animation("plunging", "deep_water", "plunging_water")

  -- Should swim.
  sol.timer.start(1500, function()

    hero:assert_position_equal(hero, destination_1)
    hero:assert_state_ground_animation("swimming", "deep_water", "swimming_stopped")

  end)
end
