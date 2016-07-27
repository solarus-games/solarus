local map = ...
local game = map:get_game()

function map:on_started()

  hero:assert_position_equal(hero, destination_1)
  hero:assert_state_ground_animation("free", "lava", "stopped")

end

function map:on_opening_transition_finished()

  sol.timer.start(100, function()
    hero:assert_position_equal(hero, destination_1)
    hero:assert_state_ground_animation("plunging", "lava", "plunging_lava")

    sol.timer.start(1000, function()

      hero:assert_position_equal(hero, destination_1)
      hero:assert_state_ground_animation("free", "lava", "stopped")
      assert(hero:is_blinking())
    end)
  end)
end
