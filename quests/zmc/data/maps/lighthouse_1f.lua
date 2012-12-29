local map = ...

function map:on_started()
  local game = map:get_game()

  game:get_item("sword"):set_variant(1)
  game:get_item("shield"):set_variant(1)

  game:get_item("lamp"):set_variant(1)
  game:get_item("magic_bar"):set_variant(1)

  game:get_item("feather"):set_variant(1)
  game:get_item("glove"):set_variant(1)

  game:get_item("bomb_bag"):set_variant(1)
  game:get_item("bombs_counter"):set_variant(1)
  game:get_item("bombs_counter"):set_amount(10)
end
