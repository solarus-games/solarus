local map = ...
-- West mountains cave 1F

function map:on_switch_activated(switch_name)

  if switch_name == "door_switch"
      and not door:is_open() then
    sol.audio.play_sound("secret")
    map:open_doors("door")
  end
end

