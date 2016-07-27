local map = ...

function map:on_started()

  -- Test an entity with one sprite.
  local block_sprite = block:get_sprite()
  assert(block_sprite ~= nil)
  local num_sprites = 0
  for sprite_name, sprite in block:get_sprites() do
    num_sprites = num_sprites + 1
    assert(sprite_name == "")
    assert(sprite ~= nil)
    assert(sprite == block_sprite)
  end
  assert_equal(num_sprites, 1)

  -- Test an entity without sprite.
  num_sprites = 0
  local destination_sprite = destination:get_sprite()
  assert(destination_sprite == nil)
  for sprite_name, sprite in destination:get_sprites() do
    num_sprites = num_sprites + 1
  end
  assert(num_sprites == 0)

  -- Test an entity with multiple sprites.
  local sprite_1 = custom_entity:create_sprite("entities/block", "my_sprite")
  local sprite_2 = custom_entity:create_sprite("entities/bomb")
  num_sprites = 0
  local main_sprite = custom_entity:get_sprite()
  assert(main_sprite ~= nil)
  assert(main_sprite == sprite_1)
  assert(main_sprite:get_animation_set() == "entities/block")

  for sprite_name, sprite in custom_entity:get_sprites() do
    num_sprites = num_sprites + 1
    if num_sprites == 1 then
      assert(sprite_name == "my_sprite")
      assert(sprite == sprite_1)
    elseif num_sprites == 2 then
      assert(sprite_name == "")
      assert(sprite == sprite_2)
    end
  end
  assert(num_sprites == 2)

  sol.main.exit()
end
