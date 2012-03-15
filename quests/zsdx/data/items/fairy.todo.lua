-- This script defines the behavior of collectable fairies present on the map.

-- A fairy appears on the map: create its movement
function self:on_pickable_appeared(pickable)

   -- create a movement that goes into random directions, with a speed of 28 pixels per second
  local movement = sol.movement.random_movement_create{speed = 28}
  movement:set_ignore_obstacles(true)
  movement:set_max_distance(40) -- don't go too far
  pickable:start_movement(movement) -- associate this movement to the fairy

  -- put the fairy on the highest layer to show it above all walls
  pickable:set_layer(2)
  pickable:set_layer_independent_collisions(true) -- but detect collisions with lower layers anyway
end

-- The direction of the movement may have changed:
-- update the direction of the fairy's sprite
function self:on_pickable_movement_changed(pickable, movement)

  if not pickable:is_following_entity() then

    local movement = pickable:get_movement()
    local sprite = pickable:get_sprite()
    local angle = movement:get_angle() -- retrieve the current movement's direction
    if angle >= 90 and angle < 270 then
      sprite:set_direction(1) -- look to the left
    else
      sprite:set_direction(0) -- look to the right
    end
  end
end

-- Obtaining a fairy
function self:on_obtaining(variant, savegame_variable)

  if sol.game:has_item("bottle_1")
      or sol.game:has_item("bottle_2")
      or sol.game:has_item("bottle_3")
      or sol.game:has_item("bottle_4") then

    -- the player has a bottle: start the dialog
    sol.map:start_dialog("found_fairy")

  else
    -- the player has no bottle: just restore 7 hearts
    sol.game:add_life(7 * 4)
  end
end

function sol.language.get_dialog("found_fairy").on_finished(answer)

  if answer ~= 1 then
    -- restore 7 hearts
    sol.game:add_life(7 * 4)
  else
    -- keep the fairy in a bottle
    local first_empty_bottle = get_first_empty_bottle()
    if first_empty_bottle == nil then
      -- no empty bottle
      sol.map:start_dialog("found_fairy.no_empty_bottle")
      sol.audio.play_sound("wrong")
    else
      -- okay, empty bottle 
      sol.game:set_item(first_empty_bottle, 6)
      sol.audio.play_sound("danger")
    end
  end
end

function sol.language.get_dialog("found_fairy.no_empty_bottle").on_finished(answer)
  -- after the 'no empty bottle' message, restore 7 hearts
  sol.game:add_life(7 * 4)
end

-- Returns the item name of an empty bottle or nil if there is no empty bottle
function get_first_empty_bottle()

  local result = ""

  if sol.game:get_item("bottle_1") == 1 then
    result = "bottle_1"
  elseif sol.game:get_item("bottle_2") == 1 then
    result = "bottle_2"
  elseif sol.game:get_item("bottle_3") == 1 then
    result = "bottle_3"
  elseif sol.game:get_item("bottle_4") == 1 then
    result = "bottle_4"
  end

  return result
end

