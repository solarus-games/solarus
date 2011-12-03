-- Dungeon 9 3F

fighting_miniboss = false

function event_map_started(destination_point_name)

  -- miniboss
  sol.map.enemy_set_group_enabled("miniboss", false)
  sol.map.door_set_open("miniboss_e_door", true)
  if sol.game.savegame_get_boolean(866) then
    sol.map.door_set_open("miniboss_door", true)
  end
end

function event_enemy_dead(enemy_name)

  -- door A
  if enemy_name:match("^door_a_enemy") then
    if not sol.map.door_is_open("door_a")
        and sol.map.enemy_is_group_dead("door_a_enemy") then
      sol.map.camera_move(2248, 648, 250, function()
	sol.main.play_sound("secret")
	sol.map.door_open("door_a")
      end)
    end

  -- miniboss
  elseif string.find(enemy_name, "^miniboss")
      and sol.map.enemy_is_group_dead("miniboss") then

    sol.main.play_music("southern_shrine.ogg")
    sol.main.play_sound("secret")
    sol.map.door_open("miniboss_door")
    sol.game.savegame_set_boolean(866, true)
  end
end

function event_npc_interaction(npc_name)

  -- door B hint stones
  if npc_name:find("^door_b_hint") then
    if not sol.map.door_is_open("door_b") then
      local door_b_next = sol.game.savegame_get_integer(1202) + 1
      local index = tonumber(npc_name:match("^door_b_hint_([1-8])$"))
      if index == door_b_next then
	-- correct
	if index < 8 then
	  local directions = { 0, 0, 0, 3, 2, 2, 2 }
	  sol.map.dialog_start("dungeon_9.3f_door_b_hint_" .. directions[index])
	  sol.game.savegame_set_integer(1202, index)
	else
	  sol.map.camera_move(1928, 1928, 500, function()
	    sol.main.play_sound("secret")
	    sol.map.door_open("door_b")
	  end)
	end
      else
	-- wrong
	sol.main.play_sound("wrong")
	sol.game.savegame_set_integer(1202, 0)
      end
    end
  end
end

function event_hero_on_sensor(sensor_name)

  -- miniboss
  if sensor_name == "start_miniboss_sensor"
      and not sol.game.savegame_get_boolean(866)
      and not fighting_miniboss then

    sol.map.hero_freeze()
    sol.map.door_close("miniboss_e_door")
    fighting_miniboss = true
    sol.main.timer_start(function()
      sol.main.play_music("boss.spc")
      sol.map.enemy_set_group_enabled("miniboss", true)
      sol.map.hero_unfreeze()
    end,
    1000)
  end
end

