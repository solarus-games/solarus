local map = ...

function map:on_opening_transition_finished()

  local x, y = hero:get_facing_position()
  assert(x == 32 and y == 24)
  assert(hero:get_facing_entity() == custom_entity)
  assert(npc:get_facing_entity() == nil)


  -- The block is facing north because its sprites only has one direction.
  assert(block:get_sprite():get_num_directions() == 1)
  assert(block:get_facing_entity() == custom_entity)

  -- The custom entity should be facing the block.
  assert(block:overlaps(custom_entity, "facing"))
  assert(custom_entity:get_sprite():get_num_directions() == 4)
  assert(custom_entity:get_facing_entity() == block)


  sol.main.exit()
end
