local map = ...
-- Sahasrahla's cave icy room

local frozen_door_sprite = nil
local frozen_door_opposite_sprite = nil

-- Function called when the map starts
function map:on_started(destination)

  if map:get_game():get_boolean(35) then
    -- remove the frozen door
    frozen_door:remove()
    frozen_door_opposite:remove()
  else
    -- initialize the direction of the frozen door sprites
    frozen_door_sprite = frozen_door:get_sprite()
    frozen_door_opposite_sprite = frozen_door_opposite:get_sprite()
    frozen_door_sprite:set_direction(3)
    frozen_door_opposite_sprite:set_direction(1)
  end
end

-- Function called when the player presses the action key on the frozen door
function frozen_door:on_interaction()
  map:start_dialog("sahasrahla_house.frozen_door")
  map:get_game():set_boolean(34, true)
end

-- Function called when the player uses an item on the frozen door
function frozen_door:on_interaction_item(item)

  if item:get_name():find("^bottle") and item:get_variant() == 2 then

    -- using water on the frozen door
    hero:freeze()
    sol.audio.play_sound("item_in_water")
    frozen_door_sprite:set_animation("disappearing")
    frozen_door_opposite_sprite:set_animation("disappearing")
    sol.timer.start(800, function()
      sol.audio.play_sound("secret")
      map:get_game():set_boolean(35, true)
      frozen_door:remove()
      frozen_door_opposite:remove()
      hero:unfreeze()
    end)
    item:set_variant(1) -- make the bottle empty
    return true
  end

  return false
end

