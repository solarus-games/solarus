local map = ...
local game = map:get_game()

function map:on_opening_transition_finished()

  -- Freeze the hero.
  hero:freeze()
  assert_equal(hero:get_state(), "frozen")

  -- Make him jump while frozen.
  local movement = sol.movement.create("jump")
  movement:set_direction8(0)
  movement:set_distance(48)
  movement:start(hero, function()

    -- He arrived on a hole and should be in state falling now.
    assert_equal(hero:get_state(), "falling")

    -- Unfreeze the hero: he is now free.
    hero:unfreeze()
    assert_equal(hero:get_state(), "free")

    -- At the next cycle, he should be falling again.
    sol.timer.start(10, function()
      assert_equal(hero:get_state(), "falling")
      sol.main.exit()
    end)
  end)

end
