-- Dungeon 4 B1

function event_map_started(destination_point_name)

  -- evil tiles
  sol.map.door_set_open("evil_tiles_door", true)
  init_evil_tiles()
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "close_door_a_sensor"
      and sol.map.door_is_open("door_a") then
    sol.map.door_close("door_a")
  elseif sensor_name:find("^evil_tiles_sensor") then
    if sol.map.door_is_open("evil_tiles_door")
        and not sol.map.enemy_is_dead("evil_tile_1") then
      sol.map.door_close("evil_tiles_door")
      sol.timer.start(2000, start_evil_tiles)
    end
  elseif sensor_name == "sensor_1" and sol.map.tile_is_enabled("sensor_1_off") then
    sol.audio.play_sound("switch")
    sol.map.tile_set_group_enabled("sensor_1_on", true)
    sol.map.tile_set_group_enabled("sensor_1_off", false)
  elseif sensor_name == "sensor_2" and sol.map.tile_is_enabled("sensor_2_off") then
    sol.audio.play_sound("switch")
    sol.map.tile_set_group_enabled("sensor_2_on", true)
    sol.map.tile_set_group_enabled("sensor_2_off", false)
  elseif sensor_name == "sensor_3" and sol.map.tile_is_enabled("sensor_3_off") then
    sol.audio.play_sound("switch")
    sol.map.tile_set_group_enabled("sensor_3_off", false)
  elseif sensor_name == "sensor_4" then
    sol.audio.play_sound("switch")
    sol.map.tile_set_group_enabled("sensor_4_on", not sol.map.tile_is_enabled("sensor_4_on"))
  elseif sensor_name == "sensor_5" and sol.map.tile_is_enabled("sensor_2_on") then
    sol.audio.play_sound("switch")
    sol.map.tile_set_group_enabled("sensor_2_on", false)
  end
end

-- Initializes evil tiles (this function should be called
-- from event_map_started)
function init_evil_tiles()
  sol.map.enemy_set_group_enabled("evil_tile", false)
  sol.map.tile_set_group_enabled("evil_tile_after", false)
end

-- Starts the attack of evil tiles
-- (this function can be called when the hero enters the room of evil tiles)
function start_evil_tiles()

  local total = sol.map.enemy_get_group_count("evil_tile")
  local next = 1 -- number of the next evil tile to spawn
  local spawn_delay = 1500 -- delay between two tiles

  sol.map.enemy_set_group_enabled("evil_tile", false)
  sol.map.tile_set_group_enabled("evil_tile_after", false)

  -- spawns a tile and schedules the next one
  function repeat_spawn()

    sol.map.enemy_set_enabled("evil_tile_"..next, true)
    sol.map.tile_set_enabled("evil_tile_after_"..next, true)
    next = next + 1
    if next <= total then
      sol.timer.start(repeat_spawn, spawn_delay)
    end
  end

  -- plays a sound repeatedly as long as at least one tile is moving
  function repeat_sound()

    sol.audio.play_sound("walk_on_grass")

    -- repeat the sound until the last tile starts animation "destroy"
    local again = false
    local remaining = sol.map.enemy_get_group_count("evil_tile")
    if remaining > 1 then
      again = true
    elseif remaining == 1 then
      local sprite = sol.map.enemy_get_sprite("evil_tile_"..total)
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

function finish_evil_tiles()

  sol.map.door_open("evil_tiles_door")
end

