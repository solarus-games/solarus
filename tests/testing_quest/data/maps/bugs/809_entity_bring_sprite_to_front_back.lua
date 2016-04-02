local map = ...

local function check_sprites(entity, expected_sprites)

  local i = 0
  for sprite_name, sprite in entity:get_sprites() do
    i = i + 1
    assert(sprite_name == expected_sprites[i][1])
    assert(sprite == expected_sprites[i][2])
  end
  assert(i == #expected_sprites)
end

function map:on_started()

  local sprite_1 = custom_entity:create_sprite("entities/block", "my_sprite")
  local sprite_2 = custom_entity:create_sprite("entities/bomb")

  check_sprites(custom_entity, {
    { "my_sprite", sprite_1 },
    { "", sprite_2 },
  })

  custom_entity:bring_sprite_to_back(sprite_1)
  check_sprites(custom_entity, {
    { "my_sprite", sprite_1 },
    { "", sprite_2 },
  })
  -- Change the order.
  custom_entity:bring_sprite_to_back(sprite_2)
  check_sprites(custom_entity, {
    { "", sprite_2 },
    { "my_sprite", sprite_1 },
  })

  custom_entity:bring_sprite_to_front(sprite_2)
  check_sprites(custom_entity, {
    { "my_sprite", sprite_1 },
    { "", sprite_2 },
  })

  custom_entity:bring_sprite_to_front(sprite_1)
  check_sprites(custom_entity, {
    { "", sprite_2 },
    { "my_sprite", sprite_1 },
  })

  sol.main.exit()
end
