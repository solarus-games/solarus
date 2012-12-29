local map = ...

function map:on_started()
  local game = map:get_game()

  game:get_item("sword"):set_variant(1)
  game:get_item("shield"):set_variant(1)
  game:get_item("lamp")
  game:get_item("feather")
  game:get_item("glove"):set_variant(1)
  game:get_item("feather")
end
