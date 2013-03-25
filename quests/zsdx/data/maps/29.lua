local map = ...
-- Twin caves

function map:on_started(destination)

  if map:get_game():get_value("b69") then
    barrier:set_enabled(false)
    barrier_switch:set_activated(true)
  end
end

function barrier_switch:on_activated()

  map:move_camera(616, 672, 250, function()
    barrier:set_enabled(false)
    sol.audio.play_sound("secret")
    map:get_game():set_value("b69", true)
  end)
end

function door_switch:on_activated()

  map:move_camera(376, 384, 250, function()
    map:open_doors("door")
    sol.audio.play_sound("secret")
  end)
end

function final_barrier_switch:on_activated()
  map:set_entities_enabled("final_barrier", false)
  sol.audio.play_sound("secret")
end

function weak_block:on_opened()

  sol.audio.play_sound("secret")
end

