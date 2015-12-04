local map = ...

function map:on_started()

  assert(hero ~= nil)

  -- Check that map:get_entities() without prefix gives the hero.
  local found = false
  for entity in map:get_entities() do
    if entity == hero then
      assert(not found)  -- Should appear only once.
      found = true
    end
  end

  assert(found)

  -- Check that map:get_entities() with prefix gives the hero.
  local found = false
  for entity in map:get_entities("h") do
    assert(not found)  -- Should appear only once.
    assert(entity == hero)
    found = true
  end

  assert(found)

  sol.main.exit()
end
