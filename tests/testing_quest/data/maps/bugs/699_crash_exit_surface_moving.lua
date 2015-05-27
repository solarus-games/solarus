local map = ...

local moving_surface

function map:on_started()

  moving_surface = sol.surface.create(320, 240)
  local movement = sol.movement.create("straight")
  movement:set_speed(64)
  movement:start(moving_surface, function() end)
end

function map:on_draw(dst_surface)

  print("map on draw")
  moving_surface:draw(dst_surface)
end

sol.timer.start(1000, sol.main.exit)