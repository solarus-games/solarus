local map = ...

function map:on_opening_transition_finished()

  local num_entities = 0
  for entity in map:get_entities_in_rectangle(hero:get_bounding_box()) do
    assert(entity == hero or entity:get_type() == "camera")
    num_entities = num_entities + 1
  end
  assert_equal(num_entities, 2)

  sol.main.exit()
end
