local map = ...
local game = map:get_game()

function map:on_started()

  hero:assert_state_ground_animation("stairs", "wall", "walking")

  sol.timer.start(3000, function()
    local hero_x, hero_y, hero_layer = hero:get_position()
    local destination_x, destination_y, destination_layer = destination_1:get_position()
    assert_equal(hero_x, destination_x)
    assert_equal(hero_y, destination_y + 8)
    assert_equal(hero_layer, destination_layer)
    hero:assert_state_ground_animation("free", "traversable", "stopped")
  end)
end
