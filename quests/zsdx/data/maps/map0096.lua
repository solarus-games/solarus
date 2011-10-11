door3 = false
function event_map_started(destination_point_name)
	sol.map.door_open("door1")
 	sol.map.door_open("door2")
	init_evil_tiles()
end

-- Initializes evil tiles (this function should be called
-- from event_map_started)
function init_evil_tiles()
  sol.map.enemy_set_group_enabled("evil", false)
  sol.map.tile_set_group_enabled("evil_after", false)
end

-- Starts the attack of evil tiles
-- (this function can be called when the hero enters the room of evil tiles)
function start_evil_tiles()

  local total = sol.map.enemy_get_group_count("evil")
  local next = 1 -- number of the next evil tile to spawn
  local spawn_delay = 1500 -- delay between two tiles

  sol.map.enemy_set_group_enabled("evil", false)
 sol.map.tile_set_group_enabled("evil_after", false)

  -- spawns a tile and schedules the next one
  function repeat_spawn()

    sol.map.enemy_set_enabled("evil_"..next, true)
    sol.map.tile_set_enabled("evil_after_"..next, true)
    next = next + 1
    if next <= total then
      sol.main.timer_start(repeat_spawn, spawn_delay)
    end
  end

  -- plays a sound repeatedly as long as at least one tile is moving
  function repeat_sound()

    sol.main.play_sound("walk_on_grass")

    -- repeat the sound until the last tile starts animation "destroy"
    local again = false
    local remaining = sol.map.enemy_get_group_count("evil")
    if remaining > 1 then
      again = true
    elseif remaining == 1 then
      local sprite = sol.map.enemy_get_sprite("evil_"..total)
      again = sol.main.sprite_get_animation(sprite) ~= "destroy"
    end

    if again then
      sol.main.timer_start(repeat_sound, 150)
    else 
     finish_evil_tiles()
    end
  end

  repeat_spawn()
  repeat_sound()
end

function event_hero_on_sensor(sensor_name)

	if sensor_name=="sensor1" then
		sol.map.door_close("door1")
		sol.map.door_close("door2")
		sol.map.sensor_set_enabled("sensor1",false)
		sol.main.timer_start(start_evil_tiles, 2000)
	end


	if sensor_name=="sensor2" then
		door3 = true
	end
end

function finish_evil_tiles()

	sol.map.door_open("door1")
	sol.map.door_open("door2")

end

function event_switch_activated(switch_name)

<<<<<<< HEAD
	if switch_name=="switch1_1" then
		if door3==false then
			sol.map.switch_set_activated("switch2_1",true)
			sol.map.door_open("door3")
			sol.main.play_sound("door_open")
 			sol.main.timer_start(function()
				if door3==false then
					sol.map.switch_set_activated("switch1_1",false)
					sol.map.door_close("door3")
					sol.main.play_sound("door_closed")
					sol.map.switch_set_activated("switch2_1",false)
				end	
			end,5000)
		end
	end

	if switch_name=="switch2_1" then
		sol.map.door_open("door3")
		sol.main.play_sound("door_open")
		sol.map.switch_set_activated("switch1_1",true)
	end

=======
>>>>>>> Lua: sol.main.door_open/close already plays the appropriate sound
end
