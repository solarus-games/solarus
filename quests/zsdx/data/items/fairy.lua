-- This script defines the behavior of collectable fairies present on the map.

-- A fairy appears on the map: create its movement
function event_appear(variant)
  movement = sol.main.random_movement_create(28)
  sol.main.movement_set_property(movement, "max_distance", 40)
  sol.item.start_movement(movement)
end

-- The direction of the movement may have changed:
-- update the direction of the fairy's sprite
function event_movement_changed()

  movement = sol.item.get_movement()
  sprite = sol.item.get_sprite()
  angle = tonumber(sol.main.movement_get_property(movement, "angle"))
  if angle >= 90 and angle < 270 then
    sol.main.sprite_set_direction(sprite, 1)
  else
    sol.main.sprite_set_direction(sprite, 0)
  end
end

-- Obtaining a fairy
function event_obtaining(variant, savegame_variable)

  if sol.game.equipment_has_item("bottle_1")
    or sol.game.equipment_has_item("bottle_2")
    or sol.game.equipment_has_item("bottle_3")
    or sol.game.equipment_has_item("bottle_4") then

    sol.map.dialog_start("found_fairy")

  else
    sol.game.equipment_add_life(7 * 4)
  end
end

function event_dialog_finished(first_message_id, answer)

  if first_message_id == "found_fairy" then

    if answer ~= 1 then
      -- restore 7 hearts
      sol.game.equipment_add_life(7 * 4)
    else
      -- keep the fairy in a bottle
      first_empty_bottle = get_first_empty_bottle()
      if first_empty_bottle == "" then
        -- no empty bottle
        sol.map.dialog_start("found_fairy.no_empty_bottle")
        sol.main.play_sound("wrong")
      else
        -- okay, empty bottle 
        sol.game.equipment_set_item(first_empty_bottle, 6)
        sol.main.play_sound("danger")
      end
    end

  elseif first_message_id == "found_fairy.no_empty_bottle" then
    sol.game.equipment_add_life(7 * 4)
  end
end

-- Returns the item name of an empty bottle, or an empty string if there is no empty bottle
function get_first_empty_bottle()

  result = ""

  if sol.game.equipment_get_item("bottle_1") == 1 then
    result = "bottle_1"
  elseif sol.game.equipment_get_item("bottle_2") == 1 then
    result = "bottle_2"
  elseif sol.game.equipment_get_item("bottle_3") == 1 then
    result = "bottle_3"
  elseif sol.game.equipment_get_item("bottle_4") == 1 then
    result = "bottle_4"
  end

  return result

end

