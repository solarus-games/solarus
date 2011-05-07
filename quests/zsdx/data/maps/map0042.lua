-- Dungeon 3 3F

function event_map_started(destination_point_name)
  sol.map.light_set(0)
end

function event_switch_activated(switch_name)

  if switch_name == "se_door_switch" and not sol.map.door_is_open("se_door") then
    sol.main.play_sound("secret")
    sol.map.door_open("se_door")
  elseif switch_name == "nc_door_switch" and not sol.map.door_is_open("nc_door") then
    sol.main.play_sound("secret")
    sol.map.door_open("nc_door")
  end
end

