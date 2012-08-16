local map = ...
-- West mountains cave 1F

function map:on_switch_activated(switch_name)

  if switch_name == "door_switch"
      and not map:door_is_open("door") then
    sol.audio.play_sound("secret")
    map:door_open("door")
  end
end

