local map = ...
-- Dungeon 10 3F

local door3_flag = false

local init_evil_tiles = sol.main.load_file("maps/evil_tiles")
init_evil_tiles(map)

function map:on_started(destination_point)

  map:set_doors_open("door1", true)
  map:set_doors_open("door2", true)
  map:set_entities_enabled("evil_tile_", false)

  if destination_point:get_name() == "entrance_F_1" then
    sensor1:set_enabled(false)
    map:set_doors_open("door3", true)
    switch1_1:set_activated(true)
  end
end

function sensor1:on_activated()

  if door1_1:is_open() then
    map:close_doors("door1")
    map:close_doors("door2")
    self:set_enabled(false)
    sol.timer.start(2000, function()
      map:start_evil_tiles()
    end)
  end
end

function sensor2:on_activated()
  door3_flag = true
end

function map:finish_evil_tiles()

  map:open_doors("door1")
  map:open_doors("door2")
end

function switch1_1:on_activated()

  if not door3_flag then
    map:open_doors("door3")
    sol.audio.play_sound("door_open")
    local timer = sol.timer.start(5000, function()
      if not door3_flag then
        self:set_activated(false)
        map:close_doors("door3")
        sol.audio.play_sound("door_closed")
      end
    end)
    timer:set_with_sound(true)
  end
end

