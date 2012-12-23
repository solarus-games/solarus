local map = ...
-- Dungeon 3 3F

function map:on_started(destination)

  map:set_light(0)

  -- weak floor
  if map:get_game():get_value("b133") then
    weak_floor:set_enabled(false)
    weak_floor_sensor:set_enabled(false)
  else
    weak_floor_teletransporter:set_enabled(false)
  end

  -- piece of heart
  if map:get_game():get_value("b132") then
    barrier:set_enabled(false)
    barrier_switch:set_activated(true)
  end
end

function se_door_switch:on_activated()

  if not se_door:is_open() then
    sol.audio.play_sound("secret")
    map:open_doors("se_door")
  end
end

function nc_door_switch:is_activated()

  if not nc_door:is_open() then
    sol.audio.play_sound("secret")
    map:open_doors("nc_door")
  end
end

function barrier_switch:on_activated()

  if barrier:is_enabled() then
    barrier:set_enabled(false)
    sol.audio.play_sound("door_open")
  end
end

function weak_floor_sensor:on_collision_explosion()

  if weak_floor:is_enabled() then

    weak_floor:set_enabled(false)
    weak_floor_sensor:set_enabled(false)
    weak_floor_teletransporter:set_enabled(true)
    sol.audio.play_sound("secret")
    map:get_game():set_value("b133", true)
  end
end

