local map = ...
-- Dungeon 8 B3

-- Legend
-- RC: Rupee Chest
-- KC: Key Chest
-- KP: Key Pot
-- LD: Locked Door
-- KD: Key Door
-- DB: Door Button
-- LB: Locked Barrier
-- BB: Barrier Button
-- DS: Door Sensor

function map:on_started(destination_point)
  map:set_doors_open("LD12", true)
  if map:get_game():get_boolean(725) then
    STT5:set_enabled(false)
  end
  if map:get_game():get_boolean(720) then
    DB1:set_activated(true)
  end
  if map:get_game():get_boolean(721) then
    DB2:set_activated(true)
  end
  if map:get_game():get_boolean(720) and map:get_game():get_boolean(721) then
    map:set_doors_open("LD14", true)
  end
end

function DS12:on_activated()
  map:close_doors("LD12")
  DS12:set_enabled(false)
end

function STT5:on_moved()
  local x, y = STT5:get_position()
  if x >= 1096 and x <= 1160
      and y >= 893 and y <= 925 then
    STT5:set_enabled(false)
    sol.audio.play_sound("jump")
    map:get_game():set_boolean(725, true)
    sol.timer.start(500, function()
      sol.audio.play_sound("bomb")
    end)
  end
end

function DB1:on_activated()

  map:get_game():set_boolean(720, true)
  if map:get_game():get_boolean(721) then
    map:open_doors("LD14")
    sol.audio.play_sound("secret")
  end
end

function DB2:on_activated()

  map:get_game():set_boolean(721, true)
  if map:get_game():get_boolean(720) then
    map:open_doors("LD14")
    sol.audio.play_sound("secret")
  end
end

function DB3:on_activated()
  map:open_doors("LD12")
  map:open_doors("LD13")
  sol.audio.play_sound("secret")
end

