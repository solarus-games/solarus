local map = ...

function map:on_started()

  local npc = map:get_entity("my_npc")
  assert(npc ~= nil)
  assert(npc:exists())
  assert(npc:get_name() == "my_npc")

  npc:remove()
  assert(not npc:exists())
  assert(map:get_entity("my_npc") == nil)
  assert(npc:get_name() == "my_npc")

  sol.timer.start(1000, function()

    assert(not npc:exists())
    assert(map:get_entity("my_npc") == nil)
    assert(npc:get_name() == "my_npc")
    sol.main.exit()

  end)

end
