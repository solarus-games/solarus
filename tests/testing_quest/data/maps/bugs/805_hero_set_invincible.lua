local map = ...

function map:on_started()

  -- Set invincible with no limit.
  hero:set_invincible(true)
  assert(hero:is_invincible())

  sol.timer.start(10, function()
    assert(hero:is_invincible())

    -- Stop invincibility.
    hero:set_invincible(false)
    assert(not hero:is_invincible())

    -- Set invincible again, this time with a limit.
    hero:set_invincible(true, 10)
    assert(hero:is_invincible())

    sol.timer.start(20, function()
      assert(not hero:is_invincible())
      sol.main.exit()
    end)
  end)
end
