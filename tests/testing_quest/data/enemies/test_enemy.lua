local enemy = ...
local game = enemy:get_game()
local map = enemy:get_map()
local hero = map:get_hero()

function enemy:on_created()

  sprite = enemy:create_sprite("enemies/test_enemy")
  enemy:set_size(16, 16)
  enemy:set_origin(8, 13)
end

-- No more behavior on purpose, this enemy is used as a blank enemy in unit tests.
