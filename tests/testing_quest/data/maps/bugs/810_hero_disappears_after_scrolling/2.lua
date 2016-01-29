local map = ...

function map:on_opening_transition_finished()

  local hero_found = false
  for entity in map:get_entities_in_rectangle(hero:get_bounding_box()) do
    assert(entity == hero)
    hero_found = true
  end

  assert(hero_found)
  sol.main.exit()
end
