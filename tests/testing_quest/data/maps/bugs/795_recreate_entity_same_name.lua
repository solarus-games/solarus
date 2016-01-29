local map = ...

function map:on_started()

  -- Create a first entity.
  local entity_1 = map:create_custom_entity({
    layer = 0,
    x = 160,
    y = 117,
    width = 16,
    height = 16,
    direction = 0,
    name = "issue_795"
  })

  assert(entity_1 ~= nil)
  assert(issue_795 ~= nil)
  assert(entity_1 == issue_795)
  assert(entity_1:get_name() == "issue_795")

  -- Remove it.
  entity_1:remove()
  assert(not entity_1:exists())

  -- Create another one with the same name.
  local entity_2 = map:create_custom_entity({
    layer = 0,
    x = 64,
    y = 64,
    width = 16,
    height = 16,
    direction = 0,
    name = "issue_795"
  })
  assert(entity_2 ~= nil)
  assert(entity_2 ~= entity_1)
  assert(entity_2:get_name() == "issue_795")
  assert(issue_795 ~= nil)
  assert(entity_2 == issue_795)

  sol.main.exit()
end
