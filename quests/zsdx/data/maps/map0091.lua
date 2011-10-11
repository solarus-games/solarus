-- West mountains cave 1F

function event_switch_activated(switch_name)

  if switch_name == "door_switch"
      and not sol.map.door_is_open("door") then
    sol.main.play_sound("secret")
    sol.map.door_open("door")
  end
end

