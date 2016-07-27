local map = ...

function map:on_started()

  local camera = map:get_camera()
  assert(camera:get_state() == "tracking")

  local movement = sol.movement.create("target")
  movement:set_target(64, 48)
  movement:set_speed(250)
  movement:start(camera)
  assert(camera:get_state() == "manual")
  sol.main.exit()
end
