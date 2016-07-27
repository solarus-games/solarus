local map = ...

function map:on_started()

  local movement = sol.movement.create("straight")
  movement:set_smooth(false)
  movement:set_angle(3 * math.pi / 2)
  movement:set_speed(96)
  movement:start(npc)

  assert(not npc:test_obstacles())
  assert(movement:get_speed() == 96)

  sol.timer.start(1000, function()
    assert(npc:test_obstacles(0, 1))  -- Obstacle one pixel to the south.
    assert(movement:get_speed() ~= 0)
    assert(movement:get_speed() == 96)
    sol.main.exit()
  end)
end
