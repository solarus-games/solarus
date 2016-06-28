local map = ...
local game = map:get_game()

function map:on_started()

  hero:assert_position_equal(hero, destination_1)
  hero:assert_state_ground_animation("free", "shallow_water", "stopped")

  local ground_sprite = hero:get_sprite("ground")
  assert(ground_sprite ~= nil)
  assert(ground_sprite:get_animation_set() == "hero/ground2")
  assert(ground_sprite:get_animation() == "walking")
end
