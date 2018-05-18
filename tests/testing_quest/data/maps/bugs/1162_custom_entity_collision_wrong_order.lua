local map = ...

function map:on_started()

  local collision_count = 0

  custom_entity:add_collision_test("sprite", function(me, other, my_sprite, other_sprite)
    collision_count = collision_count + 1
    assert(me == custom_entity)
    assert(other == enemy)
    assert(my_sprite ~= other:get_sprite())
    assert(other_sprite ~= me:get_sprite())
    assert(my_sprite == me:get_sprite())
    assert(other_sprite == other:get_sprite())
  end)

  sol.timer.start(map, 1000, function()
    assert(collision_count >= 2)
    sol.main.exit()
  end)
end
