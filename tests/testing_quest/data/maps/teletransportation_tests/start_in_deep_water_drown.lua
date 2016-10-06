local map = ...
local game = map:get_game()

function map:on_started()

  hero:assert_position_equal(hero, destination_1)
  hero:assert_state_ground_animation("plunging", "deep_water", "plunging_water")

  -- Should drown.
  sol.timer.start(1500, function()

    hero:assert_position_equal(hero, destination_1)
    assert(hero:is_blinking())
  end)

  -- Should drown repeatedly.
  sol.timer.start(100, function()
    assert(hero:get_state() == "plunging" or
        hero:get_state() == "back to solid ground")
    return true
  end)
end
