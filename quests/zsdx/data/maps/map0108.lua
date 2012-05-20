-- Dungeon 9 4F

local chest_a_timer

function event_map_started(destination_point_name)

  -- chest A
  if sol.map.get_game():get_boolean(872) then
    sol.map.switch_set_activated("chest_a_switch", true)
  else
    sol.map.chest_set_enabled("chest_a", false)
  end

  -- chest C
  if not sol.map.get_game():get_boolean(876) then
    sol.map.chest_set_enabled("chest_c", false)
  end
end

function event_switch_activated(switch_name)

  -- open doors A and B (I love you, anonymous functions)
  if switch_name == "door_ab_switch" then
    sol.map.camera_move(1288, 680, 250, function()
      -- open door A, wait 1 second and go to door B
      sol.audio.play_sound("secret")
      sol.map.door_open("door_a")
      sol.main:start_timer(1000, function()
	sol.map.camera_move(1608, 360, 250, function()
	  -- open door B
	  sol.audio.play_sound("secret")
	  sol.map.door_open("door_b")
	end) -- camera to door B
      end) -- timer before going from door A to door B
    end, 1000, 5000) -- camera to door A (don't go back too soon)

  -- chest A
  elseif switch_name == "chest_a_switch" 
      and not sol.map.chest_is_enabled("chest_a") then
    sol.map.camera_move(1336, 520, 250, function()
      sol.audio.play_sound("chest_appears")
      sol.map.chest_set_enabled("chest_a", true)
    end)
  end
end

function event_camera_back()

  local x, y = sol.map.hero_get_position()
  if y < 360 then
    -- chest A
    chest_a_timer = sol.main:start_timer(5000, true, function()
      if not sol.map.chest_is_open("chest_a") then
	sol.audio.play_sound("door_closed")
	sol.map.chest_set_enabled("chest_a", false)
	sol.map.switch_set_activated("chest_a_switch", false)
      end
    end)
  end
end

function event_treasure_obtaining(item_name, variant, savegame_variable)

  -- chest A
  if savegame_variable == 872 then
    if chest_a_timer ~= nil then
      chest_a_timer:stop()
      chest_a_timer = nil
    end
  end
end

function event_enemy_dead(enemy_name)

  if enemy_name:find("^chest_c_enemy_") then
    if sol.map.enemy_is_group_dead("chest_c_enemy")
        and not sol.map.chest_is_enabled("chest_c") then
      sol.audio.play_sound("chest_appears")
      sol.map.chest_set_enabled("chest_c", true)
    end
  end
end

function event_door_open(door_name)

  if door_name == "weak_wall_a" or door_name == "weak_wall_b" then
    sol.audio.play_sound("secret")
  end
end

