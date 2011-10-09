-- Dungeon 6 3F

function are_all_torches_on()

  return sol.map.interactive_entity_exists("torch_1")
      and sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch_1")) == "lit"
      and sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch_2")) == "lit"
end

-- Makes all torches on forever
function lock_torches()
  sol.map.interactive_entity_remove("torch_1")
  sol.map.interactive_entity_remove("torch_2")
end

function event_update()

  if not sol.map.door_is_open("torches_door")
      and are_all_torches_on() then

    sol.map.camera_move(360, 104, 250, open_torches_door)
  end
end

function open_torches_door()

  sol.main.play_sound("secret")
  sol.map.door_open("torches_door")
  lock_torches()
end

