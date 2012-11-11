local map = ...
-- Bomb cave 2F

function map:on_started(destination)

  map:set_doors_open("door", true)
  if map:get_game():get_value("130") then
    weak_floor:set_enabled(false)
    weak_floor_sensor:set_enabled(false)
  else
    weak_floor_teletransporter:set_enabled(false)
  end
end

function map:on_opening_transition_finished(destination)

  if destination:get_name() == "from_3F" then
    map:close_doors("door")
  end
end

function close_door_sensor:on_activated()

  if door:is_open() then
    map:close_doors("door")
  end
end

function weak_floor_sensor:on_collision_explosion()

  if weak_floor:is_enabled() then

    weak_floor:set_enabled(false)
    weak_floor_sensor:set_enabled(false)
    weak_floor_teletransporter:set_enabled(true)
    sol.audio.play_sound("secret")
    map:get_game():set_value("130", true)
  end
end

