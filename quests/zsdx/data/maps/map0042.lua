local map = ...
-- Dungeon 3 3F

function map:on_started(destination_point)

  map:set_light(0)

  -- weak floor
  if map:get_game():get_boolean(133) then
    weak_floor:set_enabled(false)
    weak_floor_sensor:set_enabled(false)
  else
    weak_floor_teletransporter:set_enabled(false)
  end

  -- piece of heart
  if map:get_game():get_boolean(132) then
    barrier:set_enabled(false)
    barrier_switch:set_activated(true)
  end
end

function map:on_switch_activated(switch_name)

  if switch_name == "se_door_switch" and not se_door:is_open() then
    sol.audio.play_sound("secret")
    map:open_doors("se_door")
  elseif switch_name == "nc_door_switch" and not nc_door:is_open() then
    sol.audio.play_sound("secret")
    map:open_doors("nc_door")
  elseif switch_name == "barrier_switch" and barrier:is_enabled() then
    barrier:set_enabled(false)
    sol.audio.play_sound("door_open")
  end
end

function map:on_sensor_collision_explosion(sensor_name)

  if sensor_name == "weak_floor_sensor"
      and weak_floor:is_enabled() then

    weak_floor:set_enabled(false)
    weak_floor_sensor:set_enabled(false)
    weak_floor_teletransporter:set_enabled(true)
    sol.audio.play_sound("secret")
    map:get_game():set_boolean(133, true)
  end
end

