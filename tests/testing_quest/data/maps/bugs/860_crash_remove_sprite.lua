local map = ...

function map:on_started()

  local sprite_1 = custom:create_sprite("entities/block")
  local sprite_2 = custom:create_sprite("entities/block")

  custom:remove_sprite(sprite_2)

  sol.main.exit()
end
