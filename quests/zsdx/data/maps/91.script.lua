local map = ...
-- West mountains cave 1F

function door_switch:on_activated()

  if not door:is_open() then
    sol.audio.play_sound("secret")
    map:open_doors("door")
  end
end

