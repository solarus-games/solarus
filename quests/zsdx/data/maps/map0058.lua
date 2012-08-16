local map = ...
-- Dungeon 6 1F

function map:on_started(destination_point_name)

  if map:get_game():get_boolean(313) then
    -- the torches are lit
    lock_torches()
  else
    map:chest_set_enabled("torches_chest", false)
  end
end

function map:on_map_opening_transition_finished(destination_point_name)

  -- show the welcome message
  if destination_point_name == "from_outside" then
    map:dialog_start("dungeon_6.welcome")
  end
end

function are_all_torches_on()

  return map:npc_exists("torch_1")
      and map:npc_get_sprite("torch_1"):get_animation() == "lit"
      and map:npc_get_sprite("torch_2"):get_animation() == "lit"
end

function lock_torches()
  map:npc_remove("torch_1")
  map:npc_remove("torch_2")
end

function map:on_update()

  if not map:get_game():get_boolean(313)
    and are_all_torches_on() then

    sol.audio.play_sound("chest_appears")
    map:chest_set_enabled("torches_chest", true)
    map:get_game():set_boolean(313, true)
    lock_torches()
  end
end



