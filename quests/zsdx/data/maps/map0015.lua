-- Sahasrahla's cave icy room

frozen_door_sprite = nil
frozen_door_opposite_sprite = nil

-- Function called when the map starts
function event_map_started(destination_point_name)

  if sol.map.get_game():get_boolean(35) then
    -- remove the frozen door
    sol.map.npc_remove("frozen_door")
    sol.map.npc_remove("frozen_door_opposite")
  else
    -- initialize the direction of the frozen door sprites
    frozen_door_sprite = sol.map.npc_get_sprite("frozen_door")
    frozen_door_opposite_sprite = sol.map.npc_get_sprite("frozen_door_opposite")
    frozen_door_sprite:set_direction(3)
    frozen_door_opposite_sprite:set_direction(1)
  end
end

-- Function called when the player presses the action key on the frozen door
function event_npc_interaction(npc_name)

  if npc_name == "frozen_door" then
    sol.map.dialog_start("sahasrahla_house.frozen_door")
    sol.map.get_game():set_boolean(34, true)
  end
end

-- Function called when the player uses an item on the frozen door
function event_npc_interaction_item(npc_name, item_name, variant)

  if npc_name == "frozen_door" and
      string.find(item_name, "^bottle") and variant == 2 then

    -- using water on the frozen door
    sol.map.hero_freeze()
    sol.audio.play_sound("item_in_water")
    frozen_door_sprite:set_animation("disappearing")
    frozen_door_opposite_sprite:set_animation("disappearing")
    sol.main:start_timer(800, timer_frozen_door)
    sol.map.get_game():set_item(item_name, 1) -- make the bottle empty
    return true
  end

  return false
end

-- Function called when the door is unfreezed
function timer_frozen_door()
  sol.audio.play_sound("secret")
  sol.map.get_game():set_boolean(35, true)
  sol.map.npc_remove("frozen_door")
  sol.map.npc_remove("frozen_door_opposite")
  sol.map.hero_unfreeze()
end

