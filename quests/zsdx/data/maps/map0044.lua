-- Enemy testing arena

function event_map_started(destination_point_name)

  init_evil_tiles()
  sol.main:start_timer(2000, start_evil_tiles)
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
      sol.main:start_timer(spawn_delay, repeat_spawn)
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
      sol.main:start_timer(150, repeat_sound)
    end
  end

  repeat_spawn()
  repeat_sound()
end

