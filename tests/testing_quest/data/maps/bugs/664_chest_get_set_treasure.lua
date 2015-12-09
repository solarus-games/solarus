local map = ...

local function check_chest(chest, item_name, variant, savegame_variable)

  local actual_item_name, actual_variant, actual_savegame_variable = chest:get_treasure()
  assert(actual_item_name == item_name)
  assert(actual_variant == variant)
  assert(actual_savegame_variable == savegame_variable)
end

function map:on_started()

  check_chest(empty_chest_1, nil, nil, nil)
  check_chest(empty_chest_2, nil, nil, "empty_chest")

  check_chest(chest_1, "bomb", 1, nil)
  check_chest(chest_2, "sword", 1, "sword_chest")

  chest_2:set_treasure(empty_chest_1:get_treasure())
  check_chest(chest_2, nil, nil, nil)

  empty_chest_1:set_treasure(chest_1:get_treasure())
  check_chest(empty_chest_1, "bomb", 1, nil)

  sol.main.exit()
end
