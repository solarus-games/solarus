local map = ...

function map:on_started()

  local collision_detected_by_block = false
  local collision_detected_by_chest = false

  block:add_collision_test("sprite", function(me, other, my_sprite, other_sprite)
    collision_detected_by_block = true
    assert(me == block)
    assert(other == chest)
    assert(my_sprite == me:get_sprite())
    assert(other_sprite == other:get_sprite())
  end)

  chest:add_collision_test("sprite", function(me, other, my_sprite, other_sprite)
    collision_detected_by_chest = true
    assert(me == chest)
    assert(other == block)
    assert(my_sprite == me:get_sprite())
    assert(other_sprite == other:get_sprite())
  end)

  sol.timer.start(map, 1000, function()
-- TODO    assert(collision_detected_by_block)
--    assert(collision_detected_by_chest)
    sol.main.exit()
  end)
end
