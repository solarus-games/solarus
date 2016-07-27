local map = ...

local detected = false

custom_entity_1:add_collision_test("sprite", function(_, other, this_sprite, other_sprite)
  assert(other == custom_entity_2)
  detected = true
end)

function map:on_opening_transition_finished()

  -- The collision should be detected after the opening transition,
  -- when the map is unsuspended.
  sol.timer.start(1000, function()
    assert(detected)
    sol.main.exit()
  end)
end
