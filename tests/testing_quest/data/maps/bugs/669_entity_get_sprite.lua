local map = ...

function map:on_started()

  -- Test an entity with one sprite.
  local block_sprite = block:get_sprite()
  assert(block_sprite ~= nil)
  assert(block_sprite:get_animation_set() == "entities/block")

  -- Test an entity without sprite.
  local destination_sprite = destination:get_sprite()
  assert(destination_sprite == nil)

  -- Test an entity with multiple sprites.
  local hero_tunic_sprite = hero:get_sprite()
  assert(hero_tunic_sprite ~= nil)
  assert(hero_tunic_sprite:get_animation_set() == "main_heroes/eldran")
  assert(hero_tunic_sprite:get_animation_set() == hero:get_tunic_sprite_id())
  assert(hero:get_sprite("tunic") == hero_tunic_sprite)

  local hero_shield_sprite = hero:get_sprite("shield")
  assert(hero_shield_sprite ~= hero_tunic_sprite)
  assert(hero_shield_sprite == nil)  -- No shield initially.

  sol.main.exit()
end
