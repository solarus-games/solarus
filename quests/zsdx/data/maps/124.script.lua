local map = ...

-- Hidden palace L2.

function to_layer_1_sensor:on_activated()

  local x, y = hero:get_position()
  hero:set_position(x, y, 1)
end

