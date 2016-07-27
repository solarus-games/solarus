-- Broom of the witch (custom entity).
local entity = ...
local map = entity:get_map()

function entity:on_created()
  self:set_size(16, 16)
  self:set_origin(8, 13)
  self:set_drawn_in_y_order(true)
  self:start_moving()
end

function entity:start_moving()
  local speed = 20
  local m = sol.movement.create("straight")
  m:set_speed(speed)
  m:set_angle(0)
  m:set_smooth(false)
  function m:on_obstacle_reached()
    local m2 = sol.movement.create("straight")
    m2:set_speed(speed)
    m2:set_angle(math.pi)
    m2:set_smooth(false)
    m2:start(entity)
    function m2:on_obstacle_reached()
      entity:stop_movement()
      local witch = map:get_entity("witch")
      witch:use_broom(entity)
    end
  end
  m:start(self)
end