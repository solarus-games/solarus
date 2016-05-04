local map = ...
local game = map:get_game()

function map:on_opening_transition_finished()

  local item = game:get_item("non_saved_item")
  -- hero:start_item(item) should produce a proper error
  -- about the item not saved.
  local success, error_message = pcall(hero.start_item, hero, item)
  assert(not success)
  assert(error_message:match("is not saved"))
  sol.main.exit()
end
