local item = ...

-- This script defines the behavior of collectable fairies present on the map.

-- A fairy appears on the map: create its movement
function item:on_appear(variant)

   -- create a movement that goes into random directions, with a speed of 28 pixels per second
  local movement = sol.movement.create("random")
  movement:set_speed(28)
  movement:set_ignore_obstacles(true)
  movement:set_max_distance(40) -- don't go too far
  self:start_movement(movement) -- associate this movement to the fairy

  -- put the fairy on the highest layer to show it above all walls
  local x, y = self:get_position()
  self:set_position(x, y, 2)
  self:set_layer_independent_collisions(true) -- but detect collisions with lower layers anyway
end

-- The direction of the movement may have changed:
-- update the direction of the fairy's sprite
function item:on_movement_changed()

  if not self:is_following_entity() then

    local movement = self:get_movement()
    local sprite = self:get_sprite()
    local angle = movement:get_angle() -- retrieve the current movement's direction
    if angle >= math.pi / 2 and angle < 3 * math.pi / 2 then
      sprite:set_direction(1) -- look to the left
    else
      sprite:set_direction(0) -- look to the right
    end
  end
end

-- Obtaining a fairy
function item:on_obtaining(variant, savegame_variable)

  if self:get_game():has_item("bottle_1")
      or self:get_game():has_item("bottle_2")
      or self:get_game():has_item("bottle_3")
      or self:get_game():has_item("bottle_4") then

    -- the player has a bottle: start the dialog
    self:get_map():dialog_start("found_fairy", self)

  else
    -- the player has no bottle: just restore 7 hearts
    self:get_game():add_life(7 * 4)
  end
end

function item:on_dialog_finished(dialog_id, answer)

  if dialog_id == "found_fairy" then

    if answer ~= 1 then
      -- restore 7 hearts
      self:get_game():add_life(7 * 4)
    else
      -- keep the fairy in a bottle
      local first_empty_bottle = self:get_first_empty_bottle()
      if first_empty_bottle == "" then
        -- no empty bottle
        self:get_map():dialog_start("found_fairy.no_empty_bottle", self)
        sol.audio.play_sound("wrong")
      else
        -- okay, empty bottle 
        self:get_game():set_item(first_empty_bottle, 6)
        sol.audio.play_sound("danger")
      end
    end

  elseif dialog_id == "found_fairy.no_empty_bottle" then
    -- after the 'no empty bottle' message, restore 7 hearts
    self:get_game():add_life(7 * 4)
  end
end

-- Returns the item name of an empty bottle, or an empty string if there is no empty bottle
function item:get_first_empty_bottle()

  local result = ""

  if self:get_game():get_item("bottle_1") == 1 then
    result = "bottle_1"
  elseif self:get_game():get_item("bottle_2") == 1 then
    result = "bottle_2"
  elseif self:get_game():get_item("bottle_3") == 1 then
    result = "bottle_3"
  elseif self:get_game():get_item("bottle_4") == 1 then
    result = "bottle_4"
  end

  return result

end

