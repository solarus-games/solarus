local map = ...
-- Dungeon 9 3F

fighting_miniboss = false

function map:on_started(destination_point_name)

  -- miniboss
  map:enemy_set_group_enabled("miniboss", false)
  map:door_set_open("miniboss_e_door", true)
  if map:get_game():get_boolean(866) then
    map:door_set_open("miniboss_door", true)
  end
end

function map:on_enemy_dead(enemy_name)

  -- door A
  if enemy_name:match("^door_a_enemy") then
    if not map:door_is_open("door_a")
        and map:enemy_is_group_dead("door_a_enemy") then
      map:move_camera(2248, 648, 250, function()
	sol.audio.play_sound("secret")
	map:door_open("door_a")
      end)
    end

  -- miniboss
  elseif string.find(enemy_name, "^miniboss")
      and map:enemy_is_group_dead("miniboss") then

    sol.audio.play_music("southern_shrine")
    sol.audio.play_sound("secret")
    map:door_open("miniboss_door")
    map:get_game():set_boolean(866, true)
  end
end

function map:on_npc_interaction(npc_name)

  -- door B hint stones
  if npc_name:find("^door_b_hint") then
    if not map:door_is_open("door_b") then
      local door_b_next = map:get_game():get_integer(1202) + 1
      local index = tonumber(npc_name:match("^door_b_hint_([1-8])$"))
      if index == door_b_next then
	-- correct
	if index < 8 then
	  local directions = { 0, 0, 0, 3, 2, 2, 2 }
	  map:start_dialog("dungeon_9.3f_door_b_hint_" .. directions[index])
	  map:get_game():set_integer(1202, index)
	else
	  map:move_camera(1928, 1928, 500, function()
	    sol.audio.play_sound("secret")
	    map:door_open("door_b")
	  end)
	end
      else
	-- wrong
	sol.audio.play_sound("wrong")
	map:get_game():set_integer(1202, 0)
      end
    end
  end
end

function map:on_hero_on_sensor(sensor_name)

  -- miniboss
  if sensor_name == "start_miniboss_sensor"
      and not map:get_game():get_boolean(866)
      and not fighting_miniboss then

    map:hero_freeze()
    map:door_close("miniboss_e_door")
    fighting_miniboss = true
    sol.timer.start(1000, function()
      sol.audio.play_music("boss")
      map:enemy_set_group_enabled("miniboss", true)
      map:hero_unfreeze()
    end)
  end
end

