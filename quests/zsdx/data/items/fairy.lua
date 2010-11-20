movement = nil
sprite = nil

function event_appear(variant)
  sprite = sol.item.get_sprite()
  movement = sol.main.random_movement_create(32)
  sol.main.movement_set_property(movement, "max_distance", 40)
  sol.item.start_movement(movement)
end

function event_movement_changed()

  direction360 = sol.main.movement_get_property(movement, "direction360") -- TODO
  if direction360 >= 90 and direction360 < 270 then
    sol.main.sprite_set_direction(1)
  else
    sol.main.sprite_set_direction(0)
  end
end

