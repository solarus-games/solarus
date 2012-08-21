local map = ...
-- Dungeon 10 3F

door3 = false

function map:on_started(destination_point_name)

  map:set_doors_open("door1", true)
  map:set_doors_open("door2", true)
  init_evil_tiles()

  if destination_point_name == "entrance_F_1" then
    map:sensor_set_enabled("sensor1", false)
    map:set_doors_open("door3", true)
    map:switch_set_activated("switch1_1", true)
  end
end

-- Initializes evil tiles (this function should be called
-- from on_map_started)
function init_evil_tiles()

  map:enemy_set_group_enabled("evil", false)
  map:tile_set_group_enabled("evil_after", false)
end

-- Starts the attack of evil tiles
-- (this function can be called when the hero enters the room of evil tiles)
function start_evil_tiles()

  local total = map:enemy_get_group_count("evil")
  local next = 1 -- number of the next evil tile to spawn
  local spawn_delay = 1500 -- delay between two tiles

  map:enemy_set_group_enabled("evil", false)
  map:tile_set_group_enabled("evil_after", false)

  -- spawns a tile and schedules the next one
  function repeat_spawn()

    map:enemy_set_enabled("evil_" .. next, true)
    map:tile_set_enabled("evil_after_" .. next, true)
    next = next + 1
    if next <= total then
      sol.timer.start(spawn_delay, repeat_spawn)
    end
  end

  -- plays a sound repeatedly as long as at least one tile is moving
  function repeat_sound()

    sol.audio.play_sound("walk_on_grass")

    -- repeat the sound until the last tile starts animation "destroy"
    local again = false
    local remaining = map:enemy_get_group_count("evil")
    if remaining > 1 then
      again = true
    elseif remaining == 1 then
      local sprite = map:enemy_get_sprite("evil_" .. total)
      again = sprite:get_animation() ~= "destroy"
    end

    if again then
      sol.timer.start(150, repeat_sound)
    else 
      finish_evil_tiles()
    end
  end

  repeat_spawn()
  repeat_sound()
end

function map:on_hero_on_sensor(sensor_name)

  if sensor_name == "sensor1"
      and map:door_is_open("door1_1") then
    map:close_doors("door1")
    map:close_doors("door2")
    map:sensor_set_enabled("sensor1", false)
    sol.timer.start(2000, start_evil_tiles)
  end

  if sensor_name == "sensor2" then
    door3 = true
  end
end

function finish_evil_tiles()

  map:open_doors("door1")
  map:open_doors("door2")
end

function map:on_switch_activated(switch_name)

  if switch_name == "switch1_1" then
    if not door3 then
      map:open_doors("door3")
      sol.audio.play_sound("door_open")
      sol.timer.start(5000, true, function()
        if not door3 then
          map:switch_set_activated("switch1_1", false)
          map:close_doors("door3")
          sol.audio.play_sound("door_closed")
        end
      end)
    end
  end
end

