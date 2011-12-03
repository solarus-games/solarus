-- Dungeon 9 4F

function event_map_started(destination_point_name)

  -- chest A
  if sol.game.savegame_get_boolean(872) then
    sol.map.switch_set_activated("chest_a_switch", true)
  else
    sol.map.chest_set_enabled("chest_a", false)
  end

  -- chest C
  if not sol.game.savegame_get_boolean(876) then
    sol.map.chest_set_enabled("chest_c", false)
  end
end

function event_switch_activated(switch_name)

  -- open doors A and B (I love you, anonymous functions)
  if switch_name == "door_ab_switch" then
    sol.map.camera_move(1288, 680, 250, function()
      -- open door A, wait 1 second and go to door B
      sol.main.play_sound("secret")
      sol.map.door_open("door_a")
      sol.main.timer_start(function()
	sol.map.camera_move(1608, 360, 250, function()
	  -- open door B
	  sol.main.play_sound("secret")
	  sol.map.door_open("door_b")
	end) -- camera to door B
      end, 1000) -- timer before going from door A to door B
    end, 1000, 5000) -- camera to door A (don't go back too soon)

  -- chest A
  elseif switch_name == "chest_a_switch" 
      and not sol.map.chest_is_enabled("chest_a") then
    sol.map.camera_move(1336, 520, 250, function()
      sol.main.play_sound("chest_appears")
      sol.map.chest_set_enabled("chest_a", true)
    end)
  end
end

function event_camera_back()

  local x, y = sol.map.hero_get_position()
  if y < 360 then
    -- chest A
    sol.main.timer_start(function()
      if not sol.map.chest_is_open("chest_a") then
	sol.main.play_sound("door_closed")
	sol.map.chest_set_enabled("chest_a", false)
	sol.map.switch_set_activated("chest_a_switch", false)
      end
    end, 5000, true)
  end
end

function event_treasure_obtaining(item_name, variant, savegame_variable)

  -- chest A
  if savegame_variable == 872 then
    sol.main.timer_stop_all()
  end
end

function event_enemy_dead(enemy_name)

  if enemy_name:find("^chest_c_enemy_") then
    if sol.map.enemy_is_group_dead("chest_c_enemy")
        and not sol.map.chest_is_enabled("chest_c") then
      sol.main.play_sound("chest_appears")
      sol.map.chest_set_enabled("chest_c", true)
    end
  end
end

function event_door_open(door_name)

  if door_name == "weak_wall_a" or door_name == "weak_wall_b" then
    sol.main.play_sound("secret")
  end
end

