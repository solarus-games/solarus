local enemy = ...
local game = enemy:get_game()
local map = enemy:get_map()
local hero = map:get_hero()

function enemy:on_created()

  sprite = enemy:create_sprite("enemies/slime_green")
  enemy:set_size(16, 16)
  enemy:set_obstacle_behavior("flying")
end
