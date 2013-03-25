local map = ...
-- Dungeon 9 4F

local chest_a_timer

function map:on_started(destination)

  -- chest A
  if map:get_game():get_value("b872") then
    chest_a_switch:set_activated(true)
  else
    chest_a:set_enabled(false)
  end

  -- chest C
  if not map:get_game():get_value("b876") then
    chest_c:set_enabled(false)
  end
end

-- open doors A and B (I love you, anonymous functions)
function door_ab_switch:on_activated()
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
end

-- chest A
function chest_a_switch:on_activated()

  if not chest_a:is_enabled() then
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
    chest_a_timer = sol.timer.start(5000, function()
      if not chest_a:is_open() then
	sol.audio.play_sound("door_closed")
	chest_a:set_enabled(false)
	chest_a_switch:set_activated(false)
      end
    end)
    chest_a_timer:set_with_sound(true)
  end
end

function map:on_obtaining_treasure(item, variant, savegame_variable)

  -- chest A
  if savegame_variable == "b872" then
    if chest_a_timer ~= nil then
      chest_a_timer:stop()
      chest_a_timer = nil
    end
  end
end

function chest_c_enemy_1:on_dead()

  if not map:has_entities("chest_c_enemy")
    and not chest_c:is_enabled() then
    sol.audio.play_sound("chest_appears")
    chest_c:set_enabled(true)
  end
end
chest_c_enemy_2.on_dead = chest_c_enemy_1.on_dead

function weak_wall_a:on_opened()
  sol.audio.play_sound("secret")
end

function weak_wall_b:on_opened()
  sol.audio.play_sound("secret")
end
 
