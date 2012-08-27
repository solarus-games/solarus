local map = ...
-- Dungeon 9 4F

local chest_a_timer

function map:on_started(destination_point)

  -- chest A
  if map:get_game():get_boolean(872) then
    chest_a_switch:set_activated(true)
  else
    chest_a:set_enabled(false)
  end

  -- chest C
  if not map:get_game():get_boolean(876) then
    chest_c:set_enabled(false)
  end
end

function map:on_switch_activated(switch_name)

  -- open doors A and B (I love you, anonymous functions)
  if switch_name == "door_ab_switch" then
    map:move_camera(1288, 680, 250, function()
      -- open door A, wait 1 second and go to door B
      sol.audio.play_sound("secret")
      map:open_doors("door_a")
      sol.timer.start(1000, function()
	map:move_camera(1608, 360, 250, function()
	  -- open door B
	  sol.audio.play_sound("secret")
	  map:open_doors("door_b")
	end) -- camera to door B
      end) -- timer before going from door A to door B
    end, 1000, 5000) -- camera to door A (don't go back too soon)

  -- chest A
  elseif switch_name == "chest_a_switch"
      and not chest_a:is_enabled() then
    map:move_camera(1336, 520, 250, function()
      sol.audio.play_sound("chest_appears")
      chest_a:set_enabled(true)
    end)
  end
end

function map:on_camera_back()

  local x, y = hero:get_position()
  if y < 360 then
    -- chest A
    chest_a_timer = sol.timer.start(5000, true, function()
      if not chest_a:is_open() then
	sol.audio.play_sound("door_closed")
	chest_a:set_enabled(false)
	chest_a_switch:set_activated(false)
      end
    end)
  end
end

function hero:on_obtaining_treasure(item_name, variant, savegame_variable)

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
    if not map:has_entities("chest_c_enemy")
        and not chest_c:is_enabled() then
      sol.audio.play_sound("chest_appears")
      chest_c:set_enabled(true)
    end
  end
end

function map:on_door_open(door_name)

  if door_name == "weak_wall_a" or door_name == "weak_wall_b" then
    sol.audio.play_sound("secret")
  end
end

