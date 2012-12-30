local map = ...

function map:on_started()
  local game = map:get_game()

  -- Item initialization (for testing purposes)
  game:get_item("sword"):set_variant(1)
  game:get_item("shield"):set_variant(1)
  game:get_item("lamp"):set_variant(1)
  game:get_item("magic_bar"):set_variant(1)
  game:get_item("feather"):set_variant(1)
  game:get_item("glove"):set_variant(1)
  game:get_item("bomb_bag"):set_variant(1)
  game:get_item("bombs_counter"):set_variant(1)
  game:get_item("bombs_counter"):set_amount(10)

  -- Torch puzzle
  for _, torch in ipairs(map:get_entities("torch_A")) do
    torch.on_activated = torch_A_activated
  end
end

function map:on_update()
  if torch_A1 ~= nil and
  torch_A1:get_sprite():get_animation() == "lit" and
  torch_A2:get_sprite():get_animation() == "lit" and
  torch_A3:get_sprite():get_animation() == "lit" and
  torch_A4:get_sprite():get_animation() == "lit" then
    map:remove_entities("torch_A")    
    sol.audio.play_sound("secret")
    map:open_doors("torch_door")
  end
end
