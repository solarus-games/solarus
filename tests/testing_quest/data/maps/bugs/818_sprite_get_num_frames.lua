local map = ...

function map:on_started()

  local block_sprite = sol.sprite.create("entities/block")
  assert(block_sprite:get_num_frames() == 1)

  local explosion_sprite = sol.sprite.create("entities/explosion")
  assert(explosion_sprite:get_num_frames() == 9)

  sol.main.exit()
end
