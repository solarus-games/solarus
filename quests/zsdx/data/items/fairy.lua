movement = nil
sprite = nil

function event_appear(variant)
  sprite = sol.item.get_sprite()
  movement = sol.main.random_movement_create(32)
  sol.main.movement_set_property(movement, "max_distance", 40)
  sol.item.start_movement(movement)
end

function event_movement_changed()

  -- the direction of the movement may have changed, update the direction of the fairy's sprite
  angle = tonumber(sol.main.movement_get_property(movement, "angle"))
  if angle >= 90 and angle < 270 then
    sol.main.sprite_set_direction(sprite, 1)
  else
    sol.main.sprite_set_direction(sprite, 0)
  end
end

