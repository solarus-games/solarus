local map = ...
-- Sahasrahla's cave icy room

frozen_door_sprite = nil
frozen_door_opposite_sprite = nil

-- Function called when the map starts
function map:on_started(destination_point_name)

  if map:get_game():get_boolean(35) then
    -- remove the frozen door
    map:npc_remove("frozen_door")
    map:npc_remove("frozen_door_opposite")
  else
    -- initialize the direction of the frozen door sprites
    frozen_door_sprite = map:npc_get_sprite("frozen_door")
    frozen_door_opposite_sprite = map:npc_get_sprite("frozen_door_opposite")
    frozen_door_sprite:set_direction(3)
    frozen_door_opposite_sprite:set_direction(1)
  end
end

-- Function called when the player presses the action key on the frozen door
function map:on_npc_interaction(npc_name)

  if npc_name == "frozen_door" then
    map:dialog_start("sahasrahla_house.frozen_door")
    map:get_game():set_boolean(34, true)
  end
end

-- Function called when the player uses an item on the frozen door
function map:on_npc_interaction_item(npc_name, item_name, variant)

  if npc_name == "frozen_door" and
      string.find(item_name, "^bottle") and variant == 2 then

    -- using water on the frozen door
    map:hero_freeze()
    sol.audio.play_sound("item_in_water")
    frozen_door_sprite:set_animation("disappearing")
    frozen_door_opposite_sprite:set_animation("disappearing")
    sol.timer.start(800, timer_frozen_door)
    map:get_game():set_item(item_name, 1) -- make the bottle empty
    return true
  end

  return false
end

-- Function called when the door is unfreezed
function timer_frozen_door()
  sol.audio.play_sound("secret")
  map:get_game():set_boolean(35, true)
  map:npc_remove("frozen_door")
  map:npc_remove("frozen_door_opposite")
  map:hero_unfreeze()
end

