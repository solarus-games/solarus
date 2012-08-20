local map = ...
-- Dungeon 9 4F

local chest_a_timer

function map:on_started(destination_point_name)

  -- chest A
  if map:get_game():get_boolean(872) then
    map:switch_set_activated("chest_a_switch", true)
  else
    map:chest_set_enabled("chest_a", false)
  end

  -- chest C
  if not map:get_game():get_boolean(876) then
    map:chest_set_enabled("chest_c", false)
  end
end

function map:on_switch_activated(switch_name)

  -- open doors A and B (I love you, anonymous functions)
  if switch_name == "door_ab_switch" then
    map:move_camera(1288, 680, 250, function()
      -- open door A, wait 1 second and go to door B
      sol.audio.play_sound("secret")
      map:door_open("door_a")
      sol.timer.start(1000, function()
	map:move_camera(1608, 360, 250, function()
	  -- open door B
	  sol.audio.play_sound("secret")
	  map:door_open("door_b")
	end) -- camera to door B
      end) -- timer before going from door A to door B
    end, 1000, 5000) -- camera to door A (don't go back too soon)

  -- chest A
  elseif switch_name == "chest_a_switch" 
      and not map:chest_is_enabled("chest_a") then
    map:move_camera(1336, 520, 250, function()
      sol.audio.play_sound("chest_appears")
      map:chest_set_enabled("chest_a", true)
    end)
  end
end

function map:on_camera_back()

  local x, y = map:hero_get_position()
  if y < 360 then
    -- chest A
    chest_a_timer = sol.timer.start(5000, true, function()
      if not map:chest_is_open("chest_a") then
	sol.audio.play_sound("door_closed")
	map:chest_set_enabled("chest_a", false)
	map:switch_set_activated("chest_a_switch", false)
      end
    end)
  end
end

function map:on_treasure_obtaining(item_name, variant, savegame_variable)

  -- chest A
  if savegame_variable == 872 then
    if chest_a_timer ~= nil then
      chest_a_timer:stop()
      chest_a_timer = nil
    end
  end
end

function map:on_enemy_dead(enemy_name)

  if enemy_name:find("^chest_c_enemy_") then
    if map:enemy_is_group_dead("chest_c_enemy")
        and not map:chest_is_enabled("chest_c") then
      sol.audio.play_sound("chest_appears")
      map:chest_set_enabled("chest_c", true)
    end
  end
end

function map:on_door_open(door_name)

  if door_name == "weak_wall_a" or door_name == "weak_wall_b" then
    sol.audio.play_sound("secret")
  end
end

