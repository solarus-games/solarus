local map = ...
-- Dungeon 10 1F

local function are_all_torches_on()

  return torch1 ~= nil
    and torch1:get_sprite():get_animation() == "lit"
    and torch2:get_sprite():get_animation() == "lit"
    and torch3:get_sprite():get_animation() == "lit"
    and torch4:get_sprite():get_animation() == "lit"
end

local function lock_torches()

  torch1:remove()
  torch2:remove()
  torch3:remove()
  torch4:remove()
end

function map:on_started(destination)

  map:set_light(1)

  if map:get_game():get_value("b201") then
    lock_torches()
  end

  if map:get_game():get_value("b228") then
    block_13:set_enabled(false)
  else
    block_saved:set_enabled(false)
  end

  if destination:get_name() ~= "main_entrance" then
    map:set_doors_open("eye_door", true)
  end

  local enemy_group1 = map:get_entities("enemy_group1_")
  for _, enemy in ipairs(enemy_group1) do
    enemy:on_dead = function()
      if not map:has_entities("enemy_group1")
        and not map:get_game():get_value("b200") then
        map:move_camera(616, 552, 250, function()
          map:create_pickable{
	    treasure_name = "small_key",
	    treasure_variant = 1,
	    treasure_savegame_variable = "b200",
	    x = 616,
	    y = 557,
	    layer = 1
	  }
          sol.audio.play_sound("secret")
        end)
      end
    end
  end
end

function weak_door:on_opened()
  sol.audio.play_sound("secret")
end

function map:on_opening_transition_finished(destination)

  if destination:get_name() == "main_entrance" then
    map:start_dialog("dungeon_10.welcome")
  end
end

function eye_switch:on_activated()

  -- center
  if not eye_door:is_open() then
    sol.audio.play_sound("secret")
    map:open_doors("eye_door")
  end
end

function map:on_update()

  if not map:get_game():get_value("b201") and are_all_torches_on() then
    map:get_game():set_value("b201", true)
    lock_torches()
    map:move_camera(232, 488, 250, function()
      sol.audio.play_sound("secret")
      map:create_pickable{
	treasure_name = "small_key",
	treasure_variant = 1,
	treasure_savegame_variable = "b202",
	x = 232,
	y = 493,
	layer = 0
      }
    end)
  end
end

function block_13:on_moved()
  map:get_game():set_value("b228", true)
end

