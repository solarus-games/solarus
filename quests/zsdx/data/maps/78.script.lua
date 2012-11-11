local map = ...
-- Fairy cave 2F

function map:on_started(destination)

  if map:get_game():get_value("b953") then
    barrier:set_enabled(false)
    barrier_2:set_enabled(false)
    barrier_switch:set_activated(true)
  end
end

function barrier_switch:on_activated()

  map:move_camera(312, 568, 250, function()
    sol.audio.play_sound("secret")
    map:get_game():set_value("b953", true)
    barrier:set_enabled(false)
    barrier_2:set_enabled(false)
  end)
end

